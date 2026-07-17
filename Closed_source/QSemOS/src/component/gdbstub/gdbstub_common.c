/*
 * Copyright (c) 2023-2023 Huawei Technologies Co., Ltd. All rights reserved.
 *
 * UniProton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * 	http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * See the Mulan PSL v2 for more details.
 * Create: 2023-09-14
 * Description: gdbstub通用部分，包括流程控制，软件断点管理等
 */

#include <stddef.h>
#include <errno.h>
#include <securec.h>
#include "prt_typedef.h"
#include "prt_gdbstub_ext.h"
#include "prt_notifier.h"
#include "ringbuffer.h"
#include "rsp_utils.h"
#include "arch_interface.h"
#include "gdbstub_common.h"
#include "prt_buildef.h"
#include "prt_cpu_external.h"
#include "prt_atomic.h"

#define GDB_EXCEPTION_BREAKPOINT            5
#define GDB_PACKET_SIZE                     2048

#define GDB_ENO_NOT_SUPPORT                 2

/* GDB remote serial protocol does not define errors value properly
 * and handle all error packets as the same the code error is not
 * used. There are informal values used by others gdbstub
 * implementation, like qemu. Lets use the same here.
 */
#define GDB_ERROR_GENERAL   "E01"
#define GDB_ERROR_MEMORY    "E14"
#define GDB_ERROR_INVAL     "E22"

#define CHECK_ERROR(condition) {        \
        if ((condition)) {              \
            return -1;                  \
        }                               \
    }

#define CHECK_CHAR(c)                               \
    {                                               \
        CHECK_ERROR(ptr == NULL || *ptr != (c));    \
        ptr++;                                      \
    }

enum LoopState {
    RECEIVING,
    EXIT,
} state;

#define MAX_HANDLER_NUM 128

static STUB_DATA char g_notFirstStart;
static STUB_DATA char g_gdbActive;
static STUB_DATA volatile char g_exitDbg;
static STUB_DATA volatile int g_coreId;
static STUB_DATA volatile int g_prevCoreId;
static STUB_DATA int g_thIdx;

static STUB_DATA int g_firstCoreId = -1;
static STUB_DATA volatile U32 g_onlineBitmap = 0;

#ifdef OS_OPTION_SMP
static STUB_DATA struct Atomic32 g_onlineCores = {0};
static STUB_DATA volatile uintptr_t g_initLock = OS_SPINLOCK_UNLOCK;

static STUB_DATA volatile int g_excState[OS_MAX_CORE_NUM];
static STUB_DATA volatile uintptr_t g_dbgMasterLock = OS_SPINLOCK_UNLOCK;
static STUB_DATA volatile uintptr_t g_dbgSlaveLock = OS_SPINLOCK_UNLOCK;
static STUB_DATA volatile uintptr_t g_dbgLock = OS_SPINLOCK_UNLOCK;

static STUB_DATA struct Atomic32 g_dbgMasters = {0};
static STUB_DATA struct Atomic32 g_dbgSlaves = {0};

/* to keep track of the CPU which is doing the single stepping*/
static STUB_DATA struct Atomic32 g_ssCoreId = {-1};
static STUB_DATA volatile int g_ssFlg = 0;

static STUB_TEXT void atomic_inc(struct Atomic32 *v)
{
    OsAtomic32Add(1, v);
}

static STUB_TEXT void atomic_dec(struct Atomic32 *v)
{
    OsAtomic32Add(-1, v);
}

static STUB_TEXT void atomic_set(struct Atomic32 *v, int i)
{
    v->counter = i;
}

static STUB_TEXT int atomic_read(struct Atomic32 *v)
{
    return v->counter;
}

static STUB_TEXT bool raw_spin_trylock(volatile uintptr_t *lock)
{
    return OsSplTryLock(lock);
}

static STUB_TEXT void raw_spin_lock(volatile uintptr_t *lock)
{
    OsSplLock(lock);
}

static STUB_TEXT void raw_spin_unlock(volatile uintptr_t *lock)
{
    OsSplUnlock(lock);
}

static STUB_TEXT bool raw_spin_is_locked(volatile uintptr_t *lock)
{
    return *lock == OS_SPINLOCK_LOCK;
}
#endif

static STUB_TEXT bool OsCpuOnlineCheckMask(U8 cpu_id)
{
    return (g_onlineBitmap & (1U << cpu_id)) != 0;
}

/*
 * Holds information about breakpoints.
 */
static STUB_DATA struct GdbBkpt g_breaks[GDB_MAX_BREAKPOINTS] = {
    [0 ... GDB_MAX_BREAKPOINTS - 1] = { .state = BP_UNDEFINED }
};

static STUB_DATA U8 g_serialBuf[GDB_PACKET_SIZE];

static STUB_TEXT int HexCh2Bin(unsigned char ch)
{
    unsigned char cu = ch & 0xdf; // lowercase to uppercase
    return -1 +
        ((ch - '0' +  1) & (unsigned)((ch - '9' - 1) & ('0' - 1 - ch)) >> 8) +
        ((cu - 'A' + 11) & (unsigned)((cu - 'F' - 1) & ('A' - 1 - cu)) >> 8);
}

/*
 * While we find nice hex chars, build a long val.
 * Return number of chars processed.
 */
static STUB_TEXT int OsGdbHex2U64(char **ptr, U64 *val, int maxlen)
{
    int hex;
    int num = 0;
    int negate = 0;

    if (ptr == NULL || *ptr == NULL || val == NULL || maxlen < 0) {
        return 0;
    }

    *val = 0;

    if (**ptr == '-') {
        negate = 1;
        (*ptr)++;
    }
    while (**ptr && num < maxlen) {
        hex = HexCh2Bin(**ptr);
        if (hex < 0)
            break;

        *val = (*val << 4) | hex;
        num++;
        (*ptr)++;
    }

    if (negate)
        *val = -*val;

    return num;
}

#define CHECK_HEX(arg)  do {                                    \
    U64 v = 0;                                                  \
    OsGdbHex2U64((char **)&ptr, &v, 2 * sizeof(v));             \
    CHECK_ERROR(ptr == NULL);                                   \
    arg =( __typeof__(arg)) v;                                  \
} while(0)

#define CHECK_HEXS(arg, cnt)  do {                              \
    U64 v = 0;                                                  \
    cnt = OsGdbHex2U64((char **)&ptr, &v, 2 * sizeof(v));       \
    CHECK_ERROR(ptr == NULL);                                   \
    arg =( __typeof__(arg)) v;                                  \
} while(0)

extern const char __os_section_start[];
extern const char __os_section_end[];
extern const char __os_stub_data_start[];
extern const char __os_stub_data_end[];
extern const char __os_stub_text_start[];
extern const char __os_stub_text_end[];

#define MAX_REGIONS 3
static STUB_DATA struct GdbMemRegion g_regions[MAX_REGIONS];

STUB_TEXT int OsGdbConfigInitMemRegions(void)
{
    g_regions[0].start = (uintptr_t)__os_section_start;
    g_regions[0].end = (uintptr_t)__os_section_end;
    g_regions[0].attributes = GDB_MEM_REGION_RW;

    g_regions[1].start = (uintptr_t)__os_stub_text_start;
    g_regions[1].end = (uintptr_t)__os_stub_text_end;
    g_regions[1].attributes = GDB_MEM_REGION_NO_BKPT;

    g_regions[2].start = (uintptr_t)__os_stub_data_start;
    g_regions[2].end = (uintptr_t)__os_stub_data_start;
    g_regions[2].attributes = GDB_MEM_REGION_NO_BKPT;
}

static STUB_TEXT int GdbAddrCheck(uintptr_t addr, int len, int attr)
{
    for (int i = 0; i < MAX_REGIONS; i++) {
        if (addr >= g_regions[i].start &&
            addr + len < g_regions[i].end &&
            (g_regions[i].attributes & attr) == attr) {
            return 0;
        }
    }
    return -EINVAL;
}

static STUB_TEXT int GdbInvalidReadAddr(uintptr_t addr, int len)
{
    return GdbAddrCheck(addr, len, GDB_MEM_REGION_READ);
}

static STUB_TEXT int GdbInvalidWriteAddr(uintptr_t addr, int len)
{
    return GdbAddrCheck(addr, len, GDB_MEM_REGION_WRITE);
}

static STUB_TEXT int GdbInvalidBkptAddr(uintptr_t addr)
{
    return !GdbAddrCheck(addr, BREAK_INSTR_SIZE, GDB_MEM_REGION_NO_BKPT);
}

/*
 * Some architectures need cache flushes when we set/clear a
 * breakpoint:
 */
void __weak STUB_TEXT GdbFlushSwBkptAddr(uintptr_t addr)
{
    (void)(addr);
    /* Force flush instruction cache */
}

/*
 * SW breakpoint management:
 */
static STUB_TEXT int GdbSetSwBkpt(uintptr_t addr)
{
    int breakno = -1;
    int i;

    for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
        if (g_breaks[i].state == BP_SET &&
            g_breaks[i].addr == addr)
            return -EEXIST;
    }
    for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
        if (g_breaks[i].state == BP_REMOVED &&
            g_breaks[i].addr == addr) {
            breakno = i;
            break;
        }
    }

    if (breakno == -1) {
        for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
            if (g_breaks[i].state == BP_UNDEFINED) {
                breakno = i;
                break;
            }
        }
    }

    if (breakno == -1)
        return -E2BIG;

    g_breaks[breakno].state = BP_SET;
    g_breaks[breakno].type = BP_BREAKPOINT;
    g_breaks[breakno].addr = addr;

    return 0;
}

static STUB_TEXT int GdbActivateSwBkpts(void)
{
    int error;
    int ret = 0;
    int i;

    for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
        if (g_breaks[i].state != BP_SET)
            continue;

        error = OsGdbArchSetSwBkpt(&g_breaks[i]);
        if (error) {
            ret++;
            continue;
        }

        GdbFlushSwBkptAddr(g_breaks[i].addr);
        g_breaks[i].state = BP_ACTIVE;
    }
    return ret;
}

static STUB_TEXT int GdbDeactivateSwBkpts(void)
{
    int ret = 0;
    int i;

    for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
        if (g_breaks[i].state != BP_ACTIVE)
            continue;
        ret = OsGdbArchRemoveSwBkpt(&g_breaks[i]);

        GdbFlushSwBkptAddr(g_breaks[i].addr);
        g_breaks[i].state = BP_SET;
    }
    return ret;
}

static STUB_TEXT int GdbRemoveSwBkpt(uintptr_t addr)
{
    int i;

    for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
        if ((g_breaks[i].state == BP_SET) &&
                (g_breaks[i].addr == addr)) {
            g_breaks[i].state = BP_REMOVED;
            return 0;
        }
    }
    return -ENOENT;
}

static STUB_TEXT int GdbResetBkpts(void)
{
    int i;

    /* Clear memory breakpoints. */
    for (i = 0; i < GDB_MAX_BREAKPOINTS; i++) {
        g_breaks[i].state = BP_UNDEFINED;
    }

    return 0;
}

INLINE int GdbNotSupportBkptType(U8 type) {
    return type != BP_BREAKPOINT &&
           type != BP_WRITE_WATCHPOINT &&
           type != BP_ACCESS_WATCHPOINT;
}

static STUB_TEXT int GdbAddBkpt(U8 type, uintptr_t addr, U32 kind)
{
    if (GdbNotSupportBkptType(type)) {
        return -GDB_ENO_NOT_SUPPORT;
    }
    if (GdbInvalidBkptAddr(addr)) {
        return -EINVAL;
    }
    if (type == BP_BREAKPOINT) {
        return GdbSetSwBkpt(addr);
    }
    return OsGdbArchSetHwBkpt(addr, kind, type);
}

static STUB_TEXT int GdbRemoveBkpt(U8 type, uintptr_t addr, U32 kind)
{
    if (GdbNotSupportBkptType(type)) {
        return -GDB_ENO_NOT_SUPPORT;
    }
    if (GdbInvalidBkptAddr(addr)) {
        return -EINVAL;
    }
    if (type == BP_BREAKPOINT) {
        return GdbRemoveSwBkpt(addr);
    }
    return OsGdbArchRemoveHwBkpt(addr, kind, type);
}

/* Read memory byte-by-byte */
static STUB_TEXT int GdbRawMemRead(U8 *buf, int buf_len, uintptr_t addr, int len)
{
    U8 data;
    int count = 0;
    int pos;

    /* Read from system memory */
    for (pos = 0; pos < len; pos++) {
        data = *(U8 *)(addr + pos);
        count += OsGdbBin2Hex(&data, sizeof(data), buf + count, buf_len - count);
    }

    return count;
}

/* Write memory byte-by-byte */
static STUB_TEXT int GdbRawMemWrite(const U8 *buf, uintptr_t addr, int len)
{
    U8 data;
    int count = 0;

    while (len > 0) {
        int cnt = OsGdbHex2Bin(buf, 2, &data, sizeof(data));
        if (cnt == 0) {
            return -1;
        }
        *(U8 *)addr = data;
        count += cnt;
        addr++;
        buf += 2;
        len--;
    }
    return count;
}

/*
 * Read from the memory
 * Format: m addr,length
 */
static STUB_TEXT int GdbCmdMemRead(U8 *ptr, int bufLen)
{
    (void)bufLen;
    int len;
    uintptr_t addr;
    int ret;

    CHECK_HEX(addr);
    CHECK_CHAR(',');
    CHECK_HEX(len);

    /* Read Memory */

    /*
     * GDB ask the guest to read parameters when
     * the user request backtrace. If the
     * parameter is a NULL pointer this will cause
     * a fault. Just send a packet informing that
     * this address is invalid
     */
    if (GdbInvalidReadAddr(addr, len)) {
        OsGdbSendPacket(GDB_ERROR_MEMORY, 3);
        return 0;
    }
    ret = GdbRawMemRead(g_serialBuf, sizeof(g_serialBuf), addr, len);
    CHECK_ERROR(!ret);
    OsGdbSendPacket(g_serialBuf, ret);
    return ret;
}

/*
 * Write to memory
 * Format: M addr,length:val
 */
static STUB_TEXT int GdbCmdMemWrite(U8 *ptr, int bufLen)
{
    (void)bufLen;
    int len;
    uintptr_t addr;

    CHECK_HEX(addr);
    CHECK_CHAR(',');
    CHECK_HEX(len);
    CHECK_CHAR(':');

    if (GdbInvalidWriteAddr(addr, len)) {
        OsGdbSendPacket(GDB_ERROR_MEMORY, 3);
        return 0;
    }

    /* Write Memory */
    len = GdbRawMemWrite(ptr, addr, len);
    CHECK_ERROR(len < 0);
    OsGdbSendPacket("OK", 2);
    return 0;
}

/*
 * Breakpoints
 */
static STUB_TEXT int GdbCmdBreak(U8 *ptr, int len)
{
    (void)len;
    U32 kind;
    uintptr_t addr;
    U8 type;
    int ret = 0;

    CHECK_HEX(type);
    CHECK_CHAR(',');
    CHECK_HEX(addr);
    CHECK_CHAR(',');
    CHECK_HEX(kind);

    if (g_serialBuf[0] == 'Z') {
        ret = GdbAddBkpt(type, addr, kind);
    } else if (g_serialBuf[0] == 'z') {
        ret = GdbRemoveBkpt(type, addr, kind);
    }

    if (ret == -GDB_ENO_NOT_SUPPORT) {
        /* breakpoint/watchpoint not supported */
        OsGdbSendPacket(NULL, 0);
    } else if (ret < 0) {
        OsGdbSendPacket(GDB_ERROR_INVAL, 3);
    } else {
        OsGdbSendPacket("OK", 2);
    }
    return 0;
}

/*
 * Read register
 * Format: p N
 */
static STUB_TEXT int GdbCmdReadReg(U8 *ptr, int len)
{
    (void)len;
    U32 regno;
    int ret;

    CHECK_HEX(regno);
    ret = OsGdbArchReadReg(g_coreId, regno, g_serialBuf, sizeof(g_serialBuf));
    CHECK_ERROR(ret == 0)
    OsGdbSendPacket(g_serialBuf, ret);
    return 0;
}

/*
 * Write the value of the CPU register
 * Format: P N...=XXX...
 */
static STUB_TEXT int GdbCmdWriteReg(U8 *ptr, int len)
{
    U32 regno;
    int ret;
    int cnt;

    CHECK_HEXS(regno, cnt);
    CHECK_CHAR('='); 
    ret = OsGdbArchWriteReg(g_coreId, regno, ptr, len - cnt - 1);
    CHECK_ERROR(ret < 0)
    OsGdbSendPacket("OK", 2);
    return 0;
}

static STUB_TEXT int GdbSendStopReply(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;

    // GDB_EXCEPTION_BREAKPOINT: DEBUG & BREAKPOINT:
    uintptr_t addr;
    unsigned type;
    U32 cid = OsGdbGetCoreID();
    int ret = 0;
    int stopReason = OsGdbGetStopReason();
    if (OsGdbArchHitHwBkpt(&addr, &type)) {
        const char *typeStr = GetWatchTypeStr(type);
        if (typeStr == NULL) {
            ret = sprintf_s(g_serialBuf, sizeof(g_serialBuf) - 4, "T%02xthread:%02x;", stopReason, cid);
        } else {
            ret = sprintf_s(g_serialBuf, sizeof(g_serialBuf) - 4, "T%02x%s:%x;thread:%02x;",
            stopReason, typeStr, addr, cid);
        }
    } else {
        ret = sprintf_s(g_serialBuf, sizeof(g_serialBuf) - 4, "T%02xthread:%02x;", stopReason, cid);
    }
    if (ret < 0) {
        OsGdbSendException(g_serialBuf, sizeof(g_serialBuf), stopReason);
        return 0;
    }
    OsGdbSendPacket(g_serialBuf, ret);
    return 0;
}

/* Handle the 'q' query packets */
static STUB_TEXT int GdbCmdQuery(U8 *ptr, int len)
{
    (void)len;
    int ret;
    int maxCoreNum;
    switch (*ptr) {
    case 's':
    case 'f':
        if (memcmp(&ptr[1], "ThreadInfo", 10)) {
            break;
        }
        if (*ptr == 'f') {
            g_thIdx = 0;
        }
#ifdef OS_OPTION_SMP
        maxCoreNum = atomic_read(&g_onlineCores);
#else
        maxCoreNum = 1;
#endif
        if (g_thIdx < maxCoreNum) {
            ret = sprintf_s(g_serialBuf, sizeof(g_serialBuf) - 4, "m%x", g_firstCoreId + g_thIdx);
            g_thIdx++;
            OsGdbSendPacket(g_serialBuf, ret);
        } else {
            OsGdbSendPacket("l", 1);
        }
        break;
    case 'C':
        /* return Current thread id */
        ret = sprintf_s(g_serialBuf, sizeof(g_serialBuf) - 4, "QC%x;", OsGdbGetCoreID());
        OsGdbSendPacket(g_serialBuf, ret);
        break;

    default:
        OsGdbSendPacket(NULL, 0);
        break;
    }
    return 0;
}

/* Handle the 'H' task query packets */
static STUB_TEXT int GdbCmdSetThread(U8 *ptr, int len)
{
    (void)len;
    int coreId = 0;
    U8 type = *ptr;
    ptr++;
    if (type != 'g' && type != 'c') {
        return -1;
    }
    CHECK_HEX(coreId);

    if (coreId == -1) {
        g_coreId = g_firstCoreId;
        return 0;
    }
    if (coreId < g_firstCoreId || coreId > MAX_CORE_NUM) {
        return -1;
    }

    if (!OsCpuOnlineCheckMask(coreId)) {
        return -1;
    }
    g_coreId = coreId;
    return 0;
}

/* Handle the 'T' thread query packets */
static STUB_TEXT int GdbCmdThreadAlive(U8 *ptr, int len)
{
    (void)len;
    int coreId = 0;
    CHECK_HEX(coreId);
    if (coreId < g_firstCoreId || coreId > MAX_CORE_NUM) {
        OsGdbSendPacket(GDB_ERROR_INVAL, 3);
        return 0;
    }

    if (OsCpuOnlineCheckMask(coreId)) {
        OsGdbSendPacket("OK", 2);
        return 0;
    }
    OsGdbSendPacket(GDB_ERROR_INVAL, 3);
    return 0;
}

/*
 * Continue ignoring the optional address
 * Format: c addr
 */
static STUB_TEXT int GdbCmdContinue(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    if (OsGdbArchContinue(g_coreId) != 0) {
        OsGdbSendPacket(GDB_ERROR_INVAL, 3);
        return 0;
    }
#ifdef OS_OPTION_SMP
    g_ssFlg = 0;
    atomic_set(&g_ssCoreId, -1);
    if (g_prevCoreId != g_coreId) {
        OsGdbArchContinue(g_prevCoreId);
    }
    smp_mb();
#endif
    /* We reset PC passively when receiving P/G packet. */
    OsGdbSendPacket("OK", 2);
    state = EXIT;
    return 0;
}

/*
 * Step one instruction ignoring the optional address
 * s addr..addr
 */
static STUB_TEXT int GdbCmdStep(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    if (OsGdbArchStep(g_coreId) != 0) {
        return 0;
    }
#ifdef OS_OPTION_SMP
    g_ssFlg = 1;
    atomic_set(&g_ssCoreId, g_coreId);
    if (g_prevCoreId != g_coreId) {
        OsGdbArchContinue(g_prevCoreId);
        g_ssFlg = 0;
    }
    smp_mb();
#endif
    state = EXIT;

    return 0;
}

static STUB_TEXT int GdbCmdReadAllRegs(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    int ret = OsGdbArchReadAllRegs(g_coreId, g_serialBuf, sizeof(g_serialBuf));
    CHECK_ERROR(ret == 0);
    OsGdbSendPacket(g_serialBuf, ret);
    return 0;
}

/*
 * Write the value of the CPU registers
 * Format: G XX...
 */
static STUB_TEXT int GdbCmdWriteAllRegs(U8 *ptr, int len)
{
    int ret = OsGdbArchWriteAllRegs(g_coreId, ptr, len);
    CHECK_ERROR(ret == 0);
    OsGdbSendPacket("OK", 2);
    return 0;
}

static STUB_TEXT int GdbCmdThreadSet(U8 *ptr, int len)
{
    if (*ptr == 's') {
        OsGdbSendPacket(NULL, 0);
        return 0;
    }
    if (GdbCmdSetThread(ptr, len) == 0) {
        OsGdbSendPacket("OK", 2);
    } else {
        OsGdbSendPacket(GDB_ERROR_INVAL, 3);
    }
    return 0;
}

static STUB_TEXT int GdbCmdExit(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    GdbResetBkpts();
    OsGdbArchRemoveAllHwBkpts();
    OsGdbArchContinue(g_coreId);
    state = EXIT;
    g_exitDbg = 1;
    return 0;
}

static STUB_TEXT int GdbCmdDfx(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    OsGdbSendPacket("OK", 2);
    return 0;
}

static STUB_TEXT int GdbCmdNotSupport(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    OsGdbSendPacket(NULL, 0);
    return 0;
}

static STUB_TEXT int GdbCmdNoRsp(U8 *ptr, int len)
{
    (void)ptr;
    (void)len;
    return 0;
}

typedef int (*GdbCmdHandler) (U8 *buf, int len);
typedef struct GdbCmdRegEntryT {
    char code;
    GdbCmdHandler handler;
} GdbCmdRegEntry;

static STUB_DATA GdbCmdRegEntry g_cmdRegTbl[] = {
    {'m',GdbCmdMemRead},
    {'M',GdbCmdMemWrite},
    {'c',GdbCmdContinue},
    {'s',GdbCmdStep},
    {'g',GdbCmdReadAllRegs},
    {'G',GdbCmdWriteAllRegs},
    {'p',GdbCmdReadReg},
    {'P',GdbCmdWriteReg},
    {'z',GdbCmdBreak},
    {'Z',GdbCmdBreak},
    {'?',GdbSendStopReply},
    {'H',GdbCmdThreadSet},
    {'T',GdbCmdThreadAlive},
    {'q',GdbCmdQuery},
    {'R',GdbCmdNoRsp},
    {'k',GdbCmdExit},
    {'j',GdbCmdDfx},
};

static STUB_DATA GdbCmdHandler g_gdbCmdHandlers[MAX_HANDLER_NUM];

static STUB_TEXT void GdbRegHandlers()
{
    int len = sizeof(g_cmdRegTbl) / sizeof(GdbCmdRegEntry);
    for (int i = 0; i < MAX_HANDLER_NUM; i++) {
        g_gdbCmdHandlers[i] = GdbCmdNotSupport;
    }
    for (int i = 0; i < len; i++) {
        g_gdbCmdHandlers[g_cmdRegTbl[i].code] = g_cmdRegTbl[i].handler;
    }
}

/**
 * Synchronously communicate with gdb on the host
 */
static STUB_TEXT int GdbSerialStub()
{
    state = RECEIVING;

    /* Only send exception if this is not the first GDB break. */
    if (g_notFirstStart) {
        GdbSendStopReply(NULL, 0);
    } else {
        g_notFirstStart = 1;
    }
    g_prevCoreId = g_coreId = OsGdbGetCoreID();
    while (state == RECEIVING) {
        U8 *ptr;
        int len;
        int ret;

        ret = OsGdbGetPacket(g_serialBuf, sizeof(g_serialBuf), &len);

        /* Send error and wait for next packet.*/
        if ((ret == -GDB_RSP_ENO_CHKSUM) || (ret == -GDB_RSP_ENO_2BIG)) {
            OsGdbSendPacket(GDB_ERROR_GENERAL, 3);
            continue;
        }

        if (len == 0) {
            continue;
        }

        ptr = g_serialBuf;
        char ch = *(ptr++);
        ret = g_gdbCmdHandlers[ch](ptr, len - 1);

        /*
         * If this is an recoverable error, send an error message to
         * GDB and continue the debugging session.
         */
        if (ret < 0) {
            OsGdbSendPacket(GDB_ERROR_GENERAL, 3);
            state = RECEIVING;
        }
        OsGdbFlush();
    } /* while */
    return 0;
}

#ifdef OS_OPTION_SMP

static STUB_TEXT void GdbRoundupCores(void)
{
    U32 cid;
    for (cid = g_firstCoreId; cid < OS_MAX_CORE_NUM; cid++) {
        if (cid == OsGdbGetCoreID()) {
            continue;
        }
        OsGdbArchForceStep(cid, true);
    }
    smp_mb();
}

static STUB_TEXT int GdbReturnNormal(U32 cid)
{
    if (g_coreId == g_prevCoreId || cid != g_coreId) {
        OsGdbArchContinue(cid);
    }
    os_asm_invalidate_icache_all();
    g_excState[cid] &= ~(DCPU_WANT_MASTER | DCPU_IS_SLAVE);
    smp_mb();
    atomic_dec(&g_dbgSlaves);
    return 0;
}

static STUB_TEXT int OsGdbCpuEnter(int excState)
{
    int onlineCores = atomic_read(&g_onlineCores);
    U32 cid = OsGdbGetCoreID();
    g_excState[cid] |= excState;
    if (g_exitDbg) {
        raw_spin_lock(&g_dbgLock);
        GdbDeactivateSwBkpts();
        os_asm_invalidate_icache_all();
        OsGdbArchContinue(cid);
        raw_spin_unlock(&g_dbgLock);
        return -1;
    }
    if (excState == DCPU_WANT_MASTER) {
        atomic_inc(&g_dbgMasters);
    } else {
        atomic_inc(&g_dbgSlaves);
    }
acquirelock:

    /* Make sure the above info reaches the primary CPU */
    smp_mb();

    /*
     * CPU will loop if it is a slave or request to become a master cpu:
     */
    while (1) {
        if (g_excState[cid] & DCPU_WANT_MASTER) {
            if (raw_spin_trylock(&g_dbgMasterLock)) {
                break;
            }
        } else if (g_excState[cid] & DCPU_IS_SLAVE) {
            if (!raw_spin_is_locked(&g_dbgSlaveLock)) {
                return GdbReturnNormal(cid);
            }
        } else {
            return GdbReturnNormal(cid);
        }
        cpu_relax();
    }

    /*
     * For single stepping, try to only enter on the processor that was single stepping.
     */
    if (atomic_read(&g_ssCoreId) != -1 && (cid != atomic_read(&g_ssCoreId))) {
        raw_spin_unlock(&g_dbgMasterLock);
        nop_delay(1000);
        goto acquirelock;
    }

    /*
     * Get the passive CPU lock which will hold all the non-primary
     * CPU in a spin state while the debugger is active
     */
    if (!g_ssFlg) {
        raw_spin_lock(&g_dbgSlaveLock);

        /* Signal the other CPUs to enter kgdb_wait() */
        if ((atomic_read(&g_dbgMasters) + atomic_read(&g_dbgSlaves)) != onlineCores) {
            GdbRoundupCores();
        }
    }

    /*
     * Wait for the other CPUs to be notified and be waiting for us:
     */
    while ((atomic_read(&g_dbgMasters) + atomic_read(&g_dbgSlaves)) != onlineCores) {
        nop_delay(1000);
    }

    /*
     * At this point the primary processor is completely
     * in the debugger and all secondary CPUs are quiescent
     */
    GdbDeactivateSwBkpts();
    g_ssFlg = 0;
    GdbSerialStub();
    GdbActivateSwBkpts();
    if (!g_ssFlg) {
        raw_spin_unlock(&g_dbgSlaveLock);

        /* Wait till all the CPUs have quit from the debugger. */
        while (atomic_read(&g_dbgSlaves)) {
            cpu_relax();
        }
    }

    g_excState[cid] &= ~(DCPU_WANT_MASTER | DCPU_IS_SLAVE);

    smp_mb();
    atomic_dec(&g_dbgMasters);
    raw_spin_unlock(&g_dbgMasterLock);
    return 0;
}

STUB_TEXT int OsGdbStubSmpInit(void)
{
    U32 cid = OsGdbGetCoreID();

    OsGdbSmpArchInit();
    raw_spin_lock(&g_initLock);
    atomic_inc(&g_onlineCores);
    g_onlineBitmap |= (1U << cid);
    raw_spin_unlock(&g_initLock);

    return 0;
}


STUB_TEXT void OsGdbHandleException(void *stk)
{
    int excState = OsGdbArchPrepare(stk);
    OsGdbArchDisableHwBkpts();
    OsGdbCpuEnter(excState);
    OsGdbArchCorrectHwBkpts();
    OsGdbArchFinish(stk);
}
#else
STUB_TEXT void OsGdbHandleException(void *stk)
{
    g_gdbActive = 1;
    OsGdbArchPrepare(stk);
    OsGdbArchDisableHwBkpts();
    GdbDeactivateSwBkpts();
    GdbSerialStub();
    GdbActivateSwBkpts();
    OsGdbArchCorrectHwBkpts();
    OsGdbArchFinish(stk);
    g_gdbActive = 0;
}
#endif

STUB_TEXT int OsGdbReenterChk(void *stk)
{
    (void)stk;
    return g_gdbActive;
}

static STUB_TEXT int GdbNotifyDie(struct NotifierBlock *nb,
            int action, void *data)
{
    if (g_exitDbg) {
        return NOTIFY_DONE;
    }
    return OsGdbArchNotifyDie(action, data);
}

static STUB_DATA struct NotifierBlock g_gdbNotifier = {
    .call = GdbNotifyDie,
    .priority = 999,
};

STUB_TEXT int OsGdbStubEarlyInit(void)
{
    GdbRegHandlers();
    OsGdbConfigInitMemRegions();
    if (OsGdbRingBufferInit()) {
        return -1;
    }
    OsRegisterDieNotifier(&g_gdbNotifier);
#ifdef OS_OPTION_SMP
    OsGdbStubSmpInit();
#endif
    OsGdbArchInit();
}

#ifdef OS_OPTION_SMP
extern U32 g_cfgPrimaryCore;
STUB_TEXT int OsGdbStubInit(void)
{
    U32 cid = OsGdbGetCoreID();
    if (cid == g_cfgPrimaryCore) {
        g_firstCoreId = g_cfgPrimaryCore;
        return OsGdbStubEarlyInit();
    }
    return OsGdbStubSmpInit();
}
#else
STUB_TEXT int OsGdbStubInit(void)
{
    g_firstCoreId = 0;
    return OsGdbStubEarlyInit();
}
#endif
