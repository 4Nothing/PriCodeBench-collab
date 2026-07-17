
// NOTIFY macros
#define NOTIFY_DONE 0x0000
#define NOTIFY_OK 0x0001
#define NOTIFY_STOP (NOTIFY_OK|NOTIFY_STOP_MASK)
#define NOTIFY_STOP_MASK 0x8000
#define NOTIFY_BAD (NOTIFY_STOP_MASK|0x0002)

#ifndef AUTO_STUB_PRT_PERF_H
#define AUTO_STUB_PRT_PERF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ============================================================
 * Basic types - only define if not already defined
 * ============================================================ */
#ifndef U8
typedef uint8_t U8;
#endif
#ifndef U32
typedef uint32_t U32;
#endif
#ifndef U16
typedef uint16_t U16;
#endif
#ifndef S8
typedef int8_t S8;
#endif
#ifndef S16
typedef int16_t S16;
#endif
#ifndef S32
typedef int32_t S32;
#endif
#ifndef S64
typedef int64_t S64;
#endif
typedef void *VirtAddr;
typedef void *PhyAddr;

/* ---- Structural macros (used in type definitions) ---- */
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_CORE_STR_MAX_LEN 4
/* From: src/core/kernel/include/prt_plist_external.h */
#define OS_GET_WORD_NUM_BY_PRIONUM(prio) (((prio) + 0x1f) >> 5)
/* From: src/include/uapi/prt_buildef.h */
#ifndef OS_OPTION_SMP
#define OS_OPTION_SMP 1
#endif
/* From: src/include/uapi/prt_task.h */
#ifndef OS_TSK_NAME_LEN
#define OS_TSK_NAME_LEN 16
#endif
/* From: src/include/uapi/prt_buildef.h */
#define OS_TSK_NUM_OF_PRIORITIES 32
/* From: src/extended/include/prt_perf_comm.h */
#define PRT_KERNEL_CORE_NUM 1

/* ---- Types ---- */
/* From: src/core/kernel/include/prt_sys_external.h */
typedef void (*TickDispFunc)(void);
typedef void (*TickEntryFunc)(void);
typedef void (*TaskScanFunc)(void);

/* From: src/core/kernel/include/prt_task_external.h */
typedef void (*TskCoresleep)(void);
typedef void (*TaskNameGetFunc)(U32 taskId, char **taskName);
typedef U32 (*TaskNameAddFunc)(U32 taskId, const char *name);

/* PowerOffFuncT and SetOfflineFlagFuncT - use existing definitions from qsem_test_types.h */

/* From: src/core/kernel/include/prt_tick_external.h */
typedef void(*SwitchScanFunc)(void);
typedef void (*TskmonTickHook)(void);
typedef U64 (*GetNearestTickFunc)(U32 coreID);
typedef bool (*CheckTickProcessFunc)(U32 coreID);

/* From: src/extended/include/prt_perf_comm.h */
/* PerfRegs - use definition from qsem_test_types.h */

typedef struct {
    uintptr_t ipNr;
    uintptr_t ip[PERF_MAX_CALLCHAIN_DEPTH];
} PerfBackTrace;

/* Event - use definition from qsem_test_types.h */

/* From: src/extended/perf/prt_perf_pmu.h */
typedef struct {
    Pmu pmu;
    bool enable;
} SwPmu;

/* From: src/include/uapi/prt_err.h */
typedef void (*ErrHandleFunc)(const char *fileName, U32 lineNo, U32 errorNo, U32 paraLen, void *para);

/* From: src/include/uapi/prt_perf.h */
typedef enum {
    TRACE_SYS_FLAG          = 0x10,
    TRACE_HWI_FLAG          = 0x20,
    TRACE_TASK_FLAG         = 0x40,
    TRACE_SWTMR_FLAG        = 0x80,
    TRACE_MEM_FLAG          = 0x100,
    TRACE_QUE_FLAG          = 0x200,
    TRACE_EVENT_FLAG        = 0x400,
    TRACE_SEM_FLAG          = 0x800,
    TRACE_MUX_FLAG          = 0x1000,
    TRACE_MAX_FLAG          = 0x80000000,
    TRACE_USER_DEFAULT_FLAG = 0xFFFFFFF0,
} PRT_TRACE_MASK;

typedef void (*PERF_BUF_NOTIFY_HOOK)(void);
typedef void (*PERF_BUF_FLUSH_HOOK)(void *addr, U32 size);

/* PerfEventType - use enum constants from qsem_test_types.h */
enum PerfEventType {
    PERF_EVENT_TYPE_HW = 0,
    PERF_EVENT_TYPE_TIMED = 1,
    PERF_EVENT_TYPE_SW = 2,
    PERF_EVENT_TYPE_RAW = 3,
    PERF_EVENT_TYPE_MAX = 4
};

/* PerfSampleType - use constants from qsem_test_types.h */

/* From: src/include/uapi/prt_ringbuf.h */
/* RingbufStatus - RBUF_INITED is defined in qsem_test_types.h */

/* From: src/include/uapi/prt_signal.h */
typedef unsigned long signalSet;
typedef void (*_sa_handler)(int);

typedef struct {
    int si_signo;
    int si_code;
} signalInfo;

/* From: src/include/uapi/prt_sys.h */
typedef U64 (*SysTimeFunc)(void);
typedef U32 (*CoreWakeUpHook)(U32 coreId);
typedef void (*PrtIdleHook)(void);

/* From: src/include/uapi/prt_task.h */
typedef void (*TskEntryFunc)(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
typedef U32(*TskCreateHook)(TskHandle taskPid);
typedef U32(*TskDeleteHook)(TskHandle taskPid);
typedef U32(*TskSwitchHook)(TskHandle lastTaskPid, TskHandle nextTaskPid);

/* From: src/include/uapi/prt_tick.h */
typedef void (*TickHandleFunc)(void);

/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);

/* From: src/om/include/prt_hook_external.h */
typedef void (*OsFunPara0)(void);
typedef void (*OsFunPara1)(uintptr_t);
typedef void (*OsFunPara2)(uintptr_t, uintptr_t);
typedef void (*OsFunPara3)(uintptr_t, uintptr_t, uintptr_t);
typedef void (*OsFunPara4)(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
typedef void (*OsFunPara5)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);

/* From: src/core/kernel/include/prt_sys_external.h */
struct CoreNumStr {
    char coreNo[OS_CORE_STR_MAX_LEN];
};

/* From: src/core/kernel/include/prt_task_external.h */
struct TagTskMonNode {
    U64 expiredTick;
    U32 flag;
    U32 ckFlag;
    U32 ckStyles;
    U32 reserved;
};

/* From: src/include/uapi/prt_atomic.h */
/* struct OsSpinLock - not needed, using PrtSpinLock */

/* From: src/include/uapi/prt_signal.h */
struct _sigaction {
    _sa_handler saHandler;
};

/* From: src/include/uapi/prt_task.h */
struct TskModInfo {
    U32 maxNum;
    U32 defaultSize;
    U32 idleStackSize;
    U32 magicWord;
    U32 timeSliceMs;
};

struct TskStackInfo {
    uintptr_t top;
    uintptr_t bottom;
    uintptr_t sp;
    U32 currUsed;
    U32 peakUsed;
    bool ovf;
};

/* From: src/include/uapi/prt_tick.h */
struct TickModInfo {
    U32 tickPriority;
    U32 tickPerSecond;
};

/* From: src/mem/include/prt_mem_external.h */
struct TagFscMemCtrl {
    struct TagFscMemCtrl *next;
    uintptr_t size;
    uintptr_t prevSize;
    struct TagFscMemCtrl *prev;
};

/* From: src/libc/litelibc/include/bits/list_types.h */
/* TagListObject is defined in qsem_test_types.h as ListHead */

/* From: src/include/uapi/hw/armv7-m/prt_task.h */
struct TskContext {
    U32 r4;
    U32 r5;
    U32 r6;
    U32 r7;
    U32 r8;
    U32 r9;
    U32 r10;
    U32 r11;
    U32 basePri;
    U32 excReturn;
    U32 s16;
    U32 s17;
    U32 s18;
    U32 s19;
    U32 s20;
    U32 s21;
    U32 s22;
    U32 s23;
    U32 s24;
    U32 s25;
    U32 s26;
    U32 s27;
    U32 s28;
    U32 s29;
    U32 s30;
    U32 s31;
    U32 r0;
    U32 r1;
    U32 r2;
    U32 r3;
    U32 r12;
    U32 lr;
    U32 pc;
    U32 xpsr;
};

/* From:  */
enum OsScheduleType{
    OS_SCHEDULE_RT_SINGLE
};

enum SysThreadType {
    SYS_HWI,
    SYS_TASK,
    SYS_KERNEL,
    SYS_BUTT
};

enum PmuStatus {
    PERF_PMU_STOPED,
    PERF_PMU_STARTED,
};

/* HookType is defined in qsem_test_stubs.h */

enum MemArith {
    MEM_ARITH_FSC,
    MEM_ARITH_BUTT
};

enum MemAlign {
    MEM_ADDR_ALIGN_004 = 2,
    MEM_ADDR_ALIGN_008 = 3,
    MEM_ADDR_ALIGN_016 = 4,
    MEM_ADDR_ALIGN_032 = 5,
    MEM_ADDR_ALIGN_064 = 6,
    MEM_ADDR_ALIGN_128 = 7,
    MEM_ADDR_ALIGN_256 = 8,
    MEM_ADDR_ALIGN_512 = 9,
    MEM_ADDR_ALIGN_1K = 10,
    MEM_ADDR_ALIGN_2K = 11,
    MEM_ADDR_ALIGN_4K = 12,
    MEM_ADDR_ALIGN_8K = 13,
    MEM_ADDR_ALIGN_16K = 14,
    MEM_ADDR_BUTT
};

enum MoudleId {
    OS_MID_SYS = 0x0,
    OS_MID_MEM = 0x1,
    OS_MID_FSCMEM = 0x2,
    OS_MID_TSK = 0x3,
    OS_MID_SWTMR = 0x4,
    OS_MID_TICK = 0x5,
    OS_MID_CPUP = 0x6,
    OS_MID_SEM = 0x7,
    OS_MID_HWI = 0x8,
    OS_MID_HOOK = 0x9,
    OS_MID_EXC = 0xa,
    OS_MID_EVENT = 0xb,
    OS_MID_QUEUE = 0xc,
    OS_MID_TIMER = 0xd,
    OS_MID_HARDDRV = 0xe,
    OS_MID_APP = 0xf,
    OS_MID_SIGNAL = 0x10,
    OS_MID_SHELL = 0x11,
    OS_MID_SCHED = 0X12,
    OS_MID_SPL = 0X13,
    OS_MID_DYNAMIC = 0x14,
    OS_MID_LOG = 0x15,
    OS_MID_STACKTRACE = 0x16,
    OS_MID_PERF = 0x17,
    OS_MID_BUTT
};

/* PerfStatus values are defined in qsem_test_types.h */

enum PmuHwId {
    PERF_COUNT_HW_CPU_CYCLES = 0,
    PERF_COUNT_HW_INSTRUCTIONS,
    PERF_COUNT_HW_DCACHE_REFERENCES,
    PERF_COUNT_HW_DCACHE_MISSES,
    PERF_COUNT_HW_ICACHE_REFERENCES,
    PERF_COUNT_HW_ICACHE_MISSES,
    PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
    PERF_COUNT_HW_BRANCH_MISSES,
    PERF_COUNT_HW_MAX,
};

enum PmuTimedId {
    PERF_COUNT_CPU_CLOCK = 0,
};

enum PmuSwId {
    PERF_COUNT_SW_TASK_SWITCH = 1,
    PERF_COUNT_SW_HWI_RESPONSE_IN,
    PERF_COUNT_SW_MEM_ALLOC,
    PERF_COUNT_SW_SEM_PEND,
    PERF_COUNT_SW_MAX,
};

enum SemMode {
    SEM_MODE_FIFO,
    SEM_MODE_PRIOR,
    SEM_MODE_BUTT
};

enum SysRndNumType {
    OS_SYS_RND_STACK_PROTECT,
    OS_SYS_RND_BUTT
};

enum SleepMode {
    OS_TYPE_LIGHT_SLEEP = 0,
    OS_TYPE_DEEP_SLEEP,
    OS_TYPE_INVALID_SLEEP,
};

enum HookChgType {
    HOOK_ADD_FIRST,
    HOOK_DEL_LAST
};

enum PmuEventType {
    ARMV8_PMUV3_PERF_HW_PMNC_SW_INCR               = 0x00,
    ARMV8_PMUV3_PERF_HW_L1_ICACHE_REFILL           = 0x01,
    ARMV8_PMUV3_PERF_HW_ITLB_REFILL                = 0x02,
    ARMV8_PMUV3_PERF_HW_L1_DCACHE_REFILL           = 0x03,
    ARMV8_PMUV3_PERF_HW_L1_DCACHE_ACCESS           = 0x04,
    ARMV8_PMUV3_PERF_HW_DTLB_REFILL                = 0x05,
    ARMV8_PMUV3_PERF_HW_MEM_READ                   = 0x06,
    ARMV8_PMUV3_PERF_HW_MEM_WRITE                  = 0x07,
    ARMV8_PMUV3_PERF_HW_INSTR_EXECUTED             = 0x08,
    ARMV8_PMUV3_PERF_HW_EXC_TAKEN                  = 0x09,
    ARMV8_PMUV3_PERF_HW_EXC_EXECUTED               = 0x0A,
    ARMV8_PMUV3_PERF_HW_CID_WRITE                  = 0x0B,
    ARMV8_PMUV3_PERF_HW_PC_WRITE                   = 0x0C,
    ARMV8_PMUV3_PERF_HW_PC_IMM_BRANCH              = 0x0D,
    ARMV8_PMUV3_PERF_HW_PC_PROC_RETURN             = 0x0E,
    ARMV8_PMUV3_PERF_HW_MEM_UNALIGNED_ACCESS       = 0x0F,
    ARMV8_PMUV3_PERF_HW_PC_BRANCH_MIS_PRED         = 0x10,
    ARMV8_PMUV3_PERF_HW_CLOCK_CYCLES               = 0x11,
    ARMV8_PMUV3_PERF_HW_PC_BRANCH_PRED             = 0x12,
    ARMV8_PMUV3_PERF_HW_MEM_ACCESS                 = 0x13,
    ARMV8_PMUV3_PERF_HW_L1_ICACHE_ACCESS           = 0x14,
    ARMV8_PMUV3_PERF_HW_L1_DCACHE_WB               = 0x15,
    ARMV8_PMUV3_PERF_HW_L2_CACHE_ACCESS            = 0x16,
    ARMV8_PMUV3_PERF_HW_L2_CACHE_REFILL            = 0x17,
    ARMV8_PMUV3_PERF_HW_L2_CACHE_WB                = 0x18,
    ARMV8_PMUV3_PERF_HW_BUS_ACCESS                 = 0x19,
    ARMV8_PMUV3_PERF_HW_MEM_ERROR                  = 0x1A,
    ARMV8_PMUV3_PERF_HW_OP_SPEC                    = 0x1B,
    ARMV8_PMUV3_PERF_HW_TTBR_WRITE                 = 0x1C,
    ARMV8_PMUV3_PERF_HW_BUS_CYCLES                 = 0x1D,
    ARMV8_PMUV3_PERF_HW_CHAIN                      = 0x1E,
    ARMV8_PMUV3_PERF_HW_L2D_CACHE_ALLOCATE         = 0x20,
    ARMV8_PMUV3_PERF_HW_BR_RETIRED                 = 0x21,
    ARMV8_PMUV3_PERF_HW_BR__MIS_PRED_RETIRED       = 0x22,
    ARMV8_PMUV3_PERF_HW_STALL_FRONTEND             = 0x23,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND              = 0x24,
    ARMV8_PMUV3_PERF_HW_L1D_TLB                    = 0x25,
    ARMV8_PMUV3_PERF_HW_L1I_TLB                    = 0x26,
    ARMV8_PMUV3_PERF_HW_L3D_CACHE_ALLOCATE         = 0x29,
    ARMV8_PMUV3_PERF_HW_L3D_CACHE_REFILL           = 0x2A,
    ARMV8_PMUV3_PERF_HW_L3D_CACHE                  = 0x2B,
    ARMV8_PMUV3_PERF_HW_L2D_TLB_REFILL             = 0x2D,
    ARMV8_PMUV3_PERF_HW_L2D_TLB                    = 0x2F,
    ARMV8_PMUV3_PERF_HW_DTLB_WALK                  = 0x34,
    ARMV8_PMUV3_PERF_HW_ITLB_WALK                  = 0x35,
    ARMV8_PMUV3_PERF_HW_LL_CACHE_RD                = 0x36,
    ARMV8_PMUV3_PERF_HW_LL_CACHE_MISS_RD           = 0x37,
    ARMV8_PMUV3_PERF_HW_REMOTE_ACCESS_RD           = 0x38,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_LD               = 0x40,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_ST               = 0x41,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_REFILL_LD        = 0x42,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_REFILL_ST        = 0x43,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_REFILL_INNER     = 0x44,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_REFILL_OUTER     = 0x45,
    ARMV8_PMUV3_PERF_HW_L2D_CACHE_ST               = 0x51,
    ARMV8_PMUV3_PERF_HW_L2D_CACHE_REFILL_LD        = 0x52,
    ARMV8_PMUV3_PERF_HW_L2D_CACHE_REFILL_ST        = 0x53,
    ARMV8_PMUV3_PERF_HW_BUS_ACCESS_LD              = 0x60,
    ARMV8_PMUV3_PERF_HW_BUS_ACCESS_ST              = 0x61,
    ARMV8_PMUV3_PERF_HW_MEM_ACCESS_LD              = 0x66,
    ARMV8_PMUV3_PERF_HW_MEM_ACCESS_ST              = 0x67,
    ARMV8_PMUV3_PERF_HW_LD_SPEC                    = 0x70,
    ARMV8_PMUV3_PERF_HW_ST_SPEC                    = 0x71,
    ARMV8_PMUV3_PERF_HW_LDST_SPEC                  = 0x72,
    ARMV8_PMUV3_PERF_HW_DP_SPEC                    = 0x73,
    ARMV8_PMUV3_PERF_HW_ASE_SPEC                   = 0x74,
    ARMV8_PMUV3_PERF_HW_VFP_SPEC                   = 0x75,
    ARMV8_PMUV3_PERF_HW_PC_WRITE_SPEC              = 0x76,
    ARMV8_PMUV3_PERF_HW_CRYPTO_SPEC                = 0x77,
    ARMV8_PMUV3_PERF_HW_BR_IMMED_SPEC              = 0x78,
    ARMV8_PMUV3_PERF_HW_BR_RETURN_SPEC             = 0x79,
    ARMV8_PMUV3_PERF_HW_BR_INDIRECT_SPEC           = 0x7A,
    ARMV8_PMUV3_PERF_HW_EXC_IRQ                    = 0x86,
    ARMV8_PMUV3_PERF_HW_EXC_FIQ                    = 0x87,
    ARMV8_PMUV3_PERF_HW_L3D_CACHE_RD               = 0xA0,
    ARMV8_PMUV3_PERF_HW_L3D_CACHE_REFILL_RD        = 0xA2,
    ARMV8_PMUV3_PERF_HW_L3D_CACHE_REFILL_PREFETCH  = 0xC0,
    ARMV8_PMUV3_PERF_HW_L2D_CACHE_REFILL_PREFETCH  = 0xC1,
    ARMV8_PMUV3_PERF_HW_L1D_CACHE_REFILL_PREFETCH  = 0xC2,
    ARMV8_PMUV3_PERF_HW_L2D_WS_MODE                = 0xC3,
    ARMV8_PMUV3_PERF_HW_L1D_WS_MODE_ENTRY          = 0xC4,
    ARMV8_PMUV3_PERF_HW_L1D_WS_MODE                = 0xC5,
    ARMV8_PMUV3_PERF_HW_PREDECODE_ERROR            = 0xC6,
    ARMV8_PMUV3_PERF_HW_L3D_WS_MODE                = 0xC7,
    ARMV8_PMUV3_PERF_HW_BR_COND_PRED               = 0xC9,
    ARMV8_PMUV3_PERF_HW_BR_INDIRECT_MIS_PRED       = 0xCA,
    ARMV8_PMUV3_PERF_HW_BR_INDIRECT_ADDR_MIS_PRED  = 0xCB,
    ARMV8_PMUV3_PERF_HW_BR_COND_MIS_PRED           = 0xCC,
    ARMV8_PMUV3_PERF_HW_BR_INDIRECT_ADDR_PRED      = 0xCD,
    ARMV8_PMUV3_PERF_HW_BR_RETURN_ADDR_PRED        = 0xCE,
    ARMV8_PMUV3_PERF_HW_BR_RETURN_ADDR_MIS_PRED    = 0xCF,
    ARMV8_PMUV3_PERF_HW_L2D_LLWALK_TLB             = 0xD0,
    ARMV8_PMUV3_PERF_HW_L2D_LLWALK_TLB_REFILL      = 0xD1,
    ARMV8_PMUV3_PERF_HW_L2D_L2WALK_TLB             = 0xD2,
    ARMV8_PMUV3_PERF_HW_L2D_L2WALK_TLB_REFILL      = 0xD3,
    ARMV8_PMUV3_PERF_HW_L2D_S2_TLB                 = 0xD4,
    ARMV8_PMUV3_PERF_HW_L2D_S2_TLB_REFILL          = 0xD5,
    ARMV8_PMUV3_PERF_HW_L2D_CACHE_STASH_DROPPED    = 0xD6,
    ARMV8_PMUV3_PERF_HW_STALL_FRONTEND_CACHE       = 0xE1,
    ARMV8_PMUV3_PERF_HW_STALL_FRONTEND_TLB         = 0xE2,
    ARMV8_PMUV3_PERF_HW_STALL_FRONTEND_PDERR       = 0xE3,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_ILOCK        = 0xE4,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_ILOCK_AGU    = 0xE5,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_ILOCK_FPU    = 0xE6,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_LD           = 0xE7,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_ST           = 0xE8,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_LD_CACHE     = 0xE9,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_LD_TLB       = 0xEA,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_ST_STB       = 0xEB,
    ARMV8_PMUV3_PERF_HW_STALL_BACKEND_ST_TLB       = 0xEC,
    ARMV8_PMUV3_PERF_HW_MAX
};

/* From: src/extended/include/prt_perf_comm.h */
typedef struct {
    U32           cpuid;
    U32           taskId;
    U32           eventId;
    U32           period;
    U64           time;
    uintptr_t     pc;
    PerfBackTrace callChain;
} PerfSampleData;

/* From: src/extended/perf/prt_perf_pmu.h */
typedef struct {
    Pmu pmu;
    int canDivided;
    U32 cntDivided;
    void (*enable)(Event *event);
    void (*disable)(Event *event);
    void (*start)(void);
    void (*stop)(void);
    void (*clear)(void);
    void (*setPeriod)(Event *event);
    uintptr_t (*readCnt)(Event *event);
    U32 (*mapEvent)(U32 eventType, U32 reverse);
} HwPmu;

/* From: src/include/uapi/prt_perf.h */
typedef enum {
    SYS_ERROR             = TRACE_SYS_FLAG | 0,
    SYS_START             = TRACE_SYS_FLAG | 1,
    SYS_STOP              = TRACE_SYS_FLAG | 2,
    HWI_CREATE            = TRACE_HWI_FLAG | 0,
    HWI_CREATE_SHARE      = TRACE_HWI_FLAG | 1,
    HWI_DELETE            = TRACE_HWI_FLAG | 2,
    HWI_DELETE_SHARE      = TRACE_HWI_FLAG | 3,
    HWI_RESPONSE_IN       = TRACE_HWI_FLAG | 4,
    HWI_RESPONSE_OUT      = TRACE_HWI_FLAG | 5,
    HWI_ENABLE            = TRACE_HWI_FLAG | 6,
    HWI_DISABLE           = TRACE_HWI_FLAG | 7,
    HWI_TRIGGER           = TRACE_HWI_FLAG | 8,
    HWI_SETPRI            = TRACE_HWI_FLAG | 9,
    HWI_CLEAR             = TRACE_HWI_FLAG | 10,
    HWI_SETAFFINITY       = TRACE_HWI_FLAG | 11,
    HWI_SENDIPI           = TRACE_HWI_FLAG | 12,
    TASK_CREATE           = TRACE_TASK_FLAG | 0,
    TASK_PRIOSET          = TRACE_TASK_FLAG | 1,
    TASK_DELETE           = TRACE_TASK_FLAG | 2,
    TASK_SUSPEND          = TRACE_TASK_FLAG | 3,
    TASK_RESUME           = TRACE_TASK_FLAG | 4,
    TASK_SWITCH           = TRACE_TASK_FLAG | 5,
    TASK_SIGNAL           = TRACE_TASK_FLAG | 6,
    SWTMR_CREATE          = TRACE_SWTMR_FLAG | 0,
    SWTMR_DELETE          = TRACE_SWTMR_FLAG | 1,
    SWTMR_START           = TRACE_SWTMR_FLAG | 2,
    SWTMR_STOP            = TRACE_SWTMR_FLAG | 3,
    SWTMR_EXPIRED         = TRACE_SWTMR_FLAG | 4,
    MEM_ALLOC             = TRACE_MEM_FLAG | 0,
    MEM_ALLOC_ALIGN       = TRACE_MEM_FLAG | 1,
    MEM_REALLOC           = TRACE_MEM_FLAG | 2,
    MEM_FREE              = TRACE_MEM_FLAG | 3,
    MEM_INFO_REQ          = TRACE_MEM_FLAG | 4,
    MEM_INFO              = TRACE_MEM_FLAG | 5,
    QUEUE_CREATE          = TRACE_QUE_FLAG | 0,
    QUEUE_DELETE          = TRACE_QUE_FLAG | 1,
    QUEUE_RW              = TRACE_QUE_FLAG | 2,
    EVENT_CREATE          = TRACE_EVENT_FLAG | 0,
    EVENT_DELETE          = TRACE_EVENT_FLAG | 1,
    EVENT_READ            = TRACE_EVENT_FLAG | 2,
    EVENT_WRITE           = TRACE_EVENT_FLAG | 3,
    EVENT_CLEAR           = TRACE_EVENT_FLAG | 4,
    SEM_CREATE            = TRACE_SEM_FLAG | 0,
    SEM_DELETE            = TRACE_SEM_FLAG | 1,
    SEM_PEND              = TRACE_SEM_FLAG | 2,
    SEM_POST              = TRACE_SEM_FLAG | 3,
    MUX_CREATE            = TRACE_MUX_FLAG | 0,
    MUX_DELETE            = TRACE_MUX_FLAG | 1,
    MUX_PEND              = TRACE_MUX_FLAG | 2,
    MUX_POST              = TRACE_MUX_FLAG | 3,
} PERF_TRACE_TYPE;

typedef struct {
    PerfEventType type;
    struct {
        U32 eventId;
        U32 period;
    } events[PERF_MAX_EVENT];
    U32 eventsNr;
    bool predivided;
} PerfEventConfig;

/* From: src/extended/include/prt_perf_comm.h */
typedef struct {
    U32             magic;
    PerfEventType   eventType;
    U32             len;
    U32             sampleType;
    U32             sectionId;
} PerfDataHdr;

/* From: src/include/uapi/prt_ringbuf.h */
/* Ringbuf is defined in qsem_test_types.h */

/* From: src/include/uapi/prt_hook.h */
struct HookModInfo {
    U8 maxNum[(U32)OS_HOOK_TYPE_NUM];
};

/* From: src/extended/include/prt_perf_comm.h */
typedef struct {
    Event per[PERF_MAX_EVENT];
    U8 nr;
    U8 cntDivided;
} PerfEvent;

/* From: src/include/uapi/prt_sys.h */
struct SysModInfo {
    U32 systemClock;
    SysTimeFunc sysTimeHook;
    U32 cpuType;
#if defined(OS_OPTION_SMP)
#endif
    U8 coreRunNum;
    U8 coreMaxNum;
    U8 corePrimary;

#if defined(OS_OPTION_HWI_MAX_NUM_CONFIG)
#endif
    U32 hwiMaxNum;
};

/* From: src/include/uapi/prt_task.h */
typedef U32 TskStatus;

/* From: src/include/uapi/prt_sem.h */
struct SemModInfo {
    U16 maxNum;
    U16 reserved;
};

typedef uint32_t SemHandle;

/* From: src/include/uapi/prt_task.h */
typedef uint32_t TskPrior;

/* From: src/core/kernel/include/prt_rt_external.h */
struct RtActiveTskList
{
    U32 readyPrioBit[OS_GET_WORD_NUM_BY_PRIONUM(OS_TSK_NUM_OF_PRIORITIES)];
    struct TagListObject readyList[OS_TSK_NUM_OF_PRIORITIES];
};

/* From: src/core/kernel/include/prt_plist_external.h */
struct PushablTskList
{
    U32 prio;
    struct TagListObject nodeList;
};

/* From: src/core/kernel/include/prt_task_external.h */
struct TagOsTskSortedDelayList {
    volatile uintptr_t spinLock;
    U32 reserved;
    struct TagListObject tskList;
    U64 nearestTicks;
};

/* From: src/include/uapi/prt_signal.h */
typedef struct {
    signalInfo siginfo;
    struct TagListObject siglist;
} sigInfoNode;

/* From: src/core/kernel/include/prt_plist_external.h */
struct PushablTskListHead
{
    struct TagListObject nodeList;
};

/* From: src/om/include/prt_hook_external.h */
typedef void *(*MemAllocHook)(enum MoudleId mid, U8 ptNo, U32 size);

/* From: src/extended/include/prt_perf_comm.h */
typedef struct {
    U64                  startTime;
    U64                  endTime;
    enum PerfStatus      status;
    enum PmuStatus       pmuStatusPerCpu[PRT_KERNEL_CORE_NUM];
    U32                  sampleType;
    U32                  taskIds[PERF_MAX_FILTER_TSKS];
    U8                   taskIdsNr;
    bool                 needStoreToBuffer;
    bool                 taskFilterEnable;
} PerfCB;

/* From: src/include/uapi/prt_sem.h */
struct SemInfo {
    U32 count;
    U32 owner;
    enum SemMode mode;
    U32 type;
};

/* From: src/om/include/prt_hook_external.h */
typedef U32(*OsHookChgFunc)(U32 hookType, enum HookChgType chgType);

/* From: src/include/uapi/prt_perf.h */
typedef struct {
    PerfEventConfig      eventsCfg;
    U32                  taskIds[PERF_MAX_FILTER_TSKS];
    U32                  taskIdsNr;
    U32                  sampleType;
    bool                 needStoreToBuffer;
    bool                 taskFilterEnable;
} PerfConfigAttr;

/* From: src/extended/perf/prt_perf_output.h */
typedef struct {
    Ringbuf ringbuf;
    U32 waterMark;
} PerfOutputCB;

/* From: src/include/uapi/prt_task.h */
struct TskInfo {
    uintptr_t sp;
    uintptr_t pc;
    TskStatus taskStatus;
    TskPrior taskPrio;
    U32 stackSize;
    uintptr_t topOfStack;
    char name[OS_TSK_NAME_LEN];
    U32 core;
    void *entry;
    void *tcbAddr;
    uintptr_t bottom;
    U32 currUsed;
    U32 peakUsed;
    bool ovf;
    struct TskContext context;
};

struct TskInitParam {
    TskEntryFunc taskEntry;
    TskPrior taskPrio;
    U16 reserved;
    uintptr_t args[4];
    U32 stackSize;
    char *name;
    uintptr_t stackAddr;
    U16 policy;
};

/* From: src/core/kernel/include/prt_task_external.h */
typedef U32 (*OsCheckPrioritySetFunc)(struct TagTskCb *taskCB, TskPrior taskPrio);

/* From: src/core/kernel/include/prt_rt_external.h */
struct RtRq {
    bool isOverload;
    U32 nextPrio;
    U32 nrRunning;
    struct RtActiveTskList activeTsk;
    struct PushablTskListHead pushAblList;
};

/* From: src/core/kernel/include/prt_sched_external.h */
struct TagOsRunQue {
    volatile uintptr_t spinLock;
    U32 uniFlag;
    struct TagTskCb *tskCurr;
    bool needReschedule;
    U32 rqCoreId;
    U32 tskIdlePID;
    U32 nrRunning;
    U32 intCount;
    U32 tickNoRespondCnt;
    U16 uniTaskLock;
    U16 reserved;
    U32 shakeCount;
    bool online;
    U32 currntPrio;
    struct RtRq rtRq;
    struct TagScheduleClass *schedClass;
};

/* From: src/core/kernel/include/prt_task_external.h */
struct TagScheduleClass {
    void (*osEnqueueTask)(struct TagOsRunQue *runQue, struct TagTskCb *tsk, U32 flags);
    void (*osDequeueTask)(struct TagOsRunQue *runQue, struct TagTskCb *tsk, U32 flags);
    void (*osPutPrevTask)(struct TagOsRunQue *runQue, struct TagTskCb *prevTsk);
    struct TagTskCb *(*osPickNextTask)(struct TagOsRunQue *runQue);
    struct TagTskCb *(*osNextReadyTask)(struct TagOsRunQue *runQue);
};

/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))

/* From: src/core/kernel/include/prt_sys_external.h */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

/* From: src/extended/perf/prt_perf_pmu.h */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

/* From: src/arch/include/prt_cpu_external.h */
#define ASM_NOP()

/* From: src/utility/lib/include/prt_lib_external.h */
#define ATTR_ALIGN_128 __attribute__((aligned(128)))
#define ATTR_ALIGN_16 __attribute__((aligned(16)))
#define ATTR_ALIGN_32 __attribute__((aligned(32)))
#define ATTR_ALIGN_4 __attribute__((aligned(4)))
#define ATTR_ALIGN_64 __attribute__((aligned(64)))
#define ATTR_ALIGN_8 __attribute__((aligned(8)))
#define BNUM_TO_WNUM(bnum) (((bnum) + 31) >> 5)

/* From: src/extended/perf/prt_perf_pmu.h */
#define CCNT_FULL 0xFFFFFFFF
#define CCNT_PERIOD_LOWER_BOUND 0x00000000
#define CCNT_PERIOD_UPPER_BOUND 0xFFFFFF00

/* From: src/core/kernel/include/prt_task_external.h */
#define CHECK_TSK_PID(taskID) (TSK_GET_INDEX(taskID) >= g_tskMaxNum)
#define CHECK_TSK_PID_OVERFLOW(taskId) (TSK_GET_INDEX(taskId) >= (g_tskMaxNum + 1))

/* From: src/core/kernel/include/prt_list_external.h */
#define COMPLEX_OF(ptr, type, field) ((type *)((uintptr_t)(ptr) - OFFSET_OF_FIELD(type, field)))

/* From: src/include/uapi/prt_task.h */
#define COMPOSE_PID(coreid, handle)

/* From: src/utility/lib/include/prt_lib_external.h */
#define CPUMASK_AND(m1, m2) ((m1) & (m2))
#define CPUMASK_BCLR(mask ,bit) ((mask) &= ~(1U << (bit)))
#define CPUMASK_BSET(mask ,bit) ((mask) |= (1U << (bit)))

/* From: src/core/kernel/include/prt_task_external.h */
#define CPUMASK_FIRST_BIT(mask) OsGetRMB(mask)

/* From: src/utility/lib/include/prt_lib_external.h */
#define CPUMASK_HAS_BIT(mask ,bit) ((mask) & (1U << (bit)))

/* From: src/core/kernel/include/prt_sched_external.h */
#define CPU_AGENT_CPU(cpu) (cpu)

/* From: src/core/kernel/include/prt_task_external.h */
#define CPU_OVERTIME_SORT_LIST_LOCK(tskDlyBase) OS_MCMUTEX_LOCK(0, &(tskDlyBase)->spinLock)
#define CPU_OVERTIME_SORT_LIST_UNLOCK(tskDlyBase) OS_MCMUTEX_UNLOCK(0, &(tskDlyBase)->spinLock)
#define CPU_TSK_DELAY_BASE(cpu) (&g_tskSortedDelay[(cpu)])

/* From: src/arch/cpu/armv7-m/common/os_cpu_armv7_m_external.h */
#define DIV64(a, b) OsU64DivGetQuotient((a), (b))

/* From: src/include/uapi/prt_errno.h */
#define ERRNO_OS_ID (0x00U << 16)
#define ERRTYPE_ERROR (0x02U << 24)
#define ERRTYPE_FATAL (0x03U << 24)
#define ERRTYPE_NORMAL (0x00U << 24)
#define ERRTYPE_WARN (0x01U << 24)

/* From: src/include/uapi/prt_task.h */
#define GET_COREID(pid) ((U8)((pid) >> OS_TSK_TCB_INDEX_BITS))
#define GET_HANDLE(pid) ((pid) & ((1U << OS_TSK_TCB_INDEX_BITS) - 1))

/* From: src/extended/perf/prt_perf_pmu.h */
#define GET_HW_PMU(item) PRT_DL_LIST_ENTRY(item, HwPmu, pmu)

/* From: src/core/kernel/include/prt_sched_external.h */
#define GET_RUNQ(core) ((struct TagOsRunQue *)&g_runQueue[(core)])

/* From: src/core/kernel/include/prt_task_external.h */
#define GET_TASK_RQ(task) (GET_RUNQ((task)->coreID))
#define GET_TCB_HANDLE(taskPid) (GET_TCB_HANDLE_BY_TCBID(TSK_GET_INDEX((uintptr_t)(taskPid))))
#define GET_TCB_HANDLE_BY_TCBID(index) (((struct TagTskCb *)g_tskCbArray) + (uintptr_t)(index))
#define GET_TCB_PEND(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)

/* From: src/core/kernel/include/prt_rt_external.h */
#define GET_TCB_READY(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)

/* From: src/om/include/prt_hook_external.h */
#define HOOK_ADD_IRQ_LOCK(intSave)
#define HOOK_ADD_IRQ_UNLOCK(intSave) PRT_HwiRestore((intSave))
#define HOOK_DEL_IRQ_LOCK(intSave)
#define HOOK_DEL_IRQ_UNLOCK(intSave) PRT_HwiRestore((intSave))

/* From: src/core/kernel/include/prt_task_external.h */
#define IDLE_TASK_ID g_idleTaskId

/* From: src/core/kernel/include/prt_list_external.h */
#define INIT_LIST_OBJECT(object)

/* From: src/include/uapi/prt_typedef.h */
#define INLINE static __inline __attribute__((always_inline))

/* From: src/libc/musl/include/arpa/telnet.h */
#define IP 244

/* From: src/core/kernel/include/prt_task_external.h */
#define KTHREAD_TSK_STATE_SET(tsk, tskState) ((tsk)->kthreadTsk != NULL ? (tsk)->kthreadTsk->state = (tskState) : 0)
#define KTHREAD_TSK_STATE_TST(tsk, tskState) (((tsk)->kthreadTsk != NULL) && ((tsk)->kthreadTsk->state == (tskState)))

/* From: src/utility/lib/include/prt_lib_external.h */
#define LEB128_MAX_VALUE 0x7FU
#define LEB128_STEP_SIZE 0x7U

/* From: src/include/uapi/prt_typedef.h */
#define LIKELY(x) __builtin_expect(!!(x), 1)

/* From: src/core/kernel/include/prt_list_external.h */
#define LIST_COMPONENT(ptrOfList, typeOfList, fieldOfList) COMPLEX_OF(ptrOfList, typeOfList, fieldOfList)
#define LIST_FIRST_ENTITY(listForFirstEntity, typeOfList, field)
#define LIST_FOR_EACH(posOfList, listObject, typeOfList, field)
#define LIST_FOR_EACH_SAFE(posOfList, listObject, typeOfList, field)
#define LIST_LAST(object) ((object)->prev)
#define LIST_OBJECT_INIT(object) {

/* From: src/om/include/prt_err_external.h */
#define LOG_ADDR_DBG(addr) ((void)(addr), 0x0U)

/* From: src/utility/lib/include/prt_lib_external.h */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* From: src/core/kernel/include/prt_task_external.h */
#define MAX_TASK_NUM ((1U << OS_TSK_TCB_INDEX_BITS) - 2)

/* From: src/utility/lib/include/prt_lib_external.h */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* From: src/include/uapi/prt_typedef.h */
#define NO 0

/* From: src/arch/include/prt_cpu_external.h */
#define NOP1()
#define NOP4()
#define NOP8()

/* From: src/include/uapi/prt_typedef.h */
#define NULL ((void *)0)

/* From: src/core/kernel/include/prt_list_external.h */
#define OFFSET_OF_FIELD(type, field) ((uintptr_t)((uintptr_t)(&((type *)0x10)->field) - (uintptr_t)0x10))

/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_32BIT_MASK(bit) (1U << OS_32BIT_MOD(bit))
#define OS_32BIT_MOD(num) ((num) % 32)
#define OS_32BIT_VERSE_MASK(bit) (~(1U << OS_32BIT_MOD(bit)))
#define OS_64BIT_SET(high, low) (((U64)(high) << 32) + (U64)(low))
#define OS_ADDR_OVERTURN_CHK(addr, size) (((size) != 0) && (((addr) + (size) - 1) < (addr)))
#define OS_ALIGN(addr, align) ((uintptr_t)(addr) & ((align) - 1))
#define OS_ALIGN_CHK(addr, align) (((uintptr_t)(addr) & ((align) - 1)) == 0)
#define OS_BIT0_MASK (0x1U << 0)
#define OS_BIT10_MASK (0x1U << 10)
#define OS_BIT11_MASK (0x1U << 11)
#define OS_BIT12_MASK (0x1U << 12)
#define OS_BIT13_MASK (0x1U << 13)
#define OS_BIT14_MASK (0x1U << 14)
#define OS_BIT15_MASK (0x1U << 15)
#define OS_BIT16_MASK (0x1U << 16)
#define OS_BIT17_MASK (0x1U << 17)
#define OS_BIT18_MASK (0x1U << 18)
#define OS_BIT19_MASK (0x1U << 19)
#define OS_BIT20_MASK (0x1U << 20)
#define OS_BIT21_MASK (0x1U << 21)
#define OS_BIT22_MASK (0x1U << 22)
#define OS_BIT23_MASK (0x1U << 23)
#define OS_BIT24_MASK (0x1U << 24)
#define OS_BIT25_MASK (0x1U << 25)
#define OS_BIT26_MASK (0x1U << 26)
#define OS_BIT27_MASK (0x1U << 27)
#define OS_BIT28_MASK (0x1U << 28)
#define OS_BIT29_MASK (0x1U << 29)
#define OS_BIT2_MASK (0x1U << 2)
#define OS_BIT30_MASK (0x1U << 30)
#define OS_BIT31_MASK (0x1U << 31)
#define OS_BIT3_MASK (0x1U << 3)
#define OS_BIT4_MASK (0x1U << 4)
#define OS_BIT5_MASK (0x1U << 5)
#define OS_BIT6_MASK (0x1U << 6)
#define OS_BIT7_MASK (0x1U << 7)
#define OS_BIT8_MASK (0x1U << 8)
#define OS_BIT9_MASK (0x1U << 9)
#define OS_BITNUM_2_MASK(bitNum) ((1U << (bitNum)) - 1)
#define OS_BITS_PER_BYTE 8
#define OS_BIT_SET_VALUE(bit) (0x1UL << (bit))

/* From: src/include/uapi/prt_sys.h */
#define OS_BOOT_PHASE 0x03
#define OS_BSSINIT_PHASE 0x06

/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_BYTES_PER_DWORD (sizeof(U64))
#define OS_BYTES_PER_WORD (sizeof(U32))

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_CLR_CHILD_BIT_MAP(priority)
#define OS_CLR_RDY_TSK_BIT_MAP(priority)
#define OS_COREID_CHK_INVALID(coreId) ((coreId) >= g_maxNumOfCores)
#define OS_CORES_32 32
#define OS_CORE_MASK U32

/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_CORE_STR_END_INDEX (OS_CORE_STR_MAX_LEN - 1)
#define OS_CORE_STR_NUM_INDEX (OS_CORE_STR_END_INDEX -1)

/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_DECIMAL 10

/* From: src/include/uapi/prt_sys.h */
#define OS_DEFAULT_PHASE 0x00

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_DEFAULT_SCHED_TYPE OS_SCHEDULE_RT_SINGLE

/* From: src/include/uapi/prt_sys.h */
#define OS_DEVDRVINIT_PHASE 0x0b

/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_DWORD_BIT_NUM 64
#define OS_DWORD_BIT_POW 6

/* From: src/include/uapi/prt_typedef.h */
#define OS_EMBED_ASM __asm__ __volatile__

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_ENQUEUE_HEAD 1U

/* From: src/include/uapi/prt_errno.h */
#define OS_ERRNO_BUILD_ERROR(mid, errno) (ERRTYPE_ERROR | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
#define OS_ERRNO_BUILD_FATAL(mid, errno) (ERRTYPE_FATAL | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))

/* Error codes are defined in qsem_test_types.h */

/* From: src/include/uapi/prt_typedef.h */
#define OS_FAIL 1

/* From: src/arch/include/prt_asm_cpu_external.h */
#define OS_FLG_BGD_ACTIVE 0x0002
#define OS_FLG_EXC_ACTIVE 0x0020
#define OS_FLG_HWI_ACTIVE 0x0001
#define OS_FLG_SYS_ACTIVE 0x0010
#define OS_FLG_TICK_ACTIVE 0x0008

/* From: src/mem/include/prt_mem_external.h */
#define OS_FSC_MEM_LAST_IDX 31
#define OS_FSC_MEM_MAGIC_USED (struct TagFscMemCtrl *)0x5a5aa5a5
#define OS_FSC_MEM_MAXVAL ((1U << OS_FSC_MEM_LAST_IDX) - OS_FSC_MEM_SIZE_ALIGN)

/* From: src/mem/fsc/prt_fscmem_internal.h */
#define OS_FSC_MEM_SIZE_ALIGN OS_MEM_ADDR_ALIGN

/* From: src/mem/include/prt_mem_external.h */
#define OS_FSC_MEM_SZGET(currBlk) ((U32)(currBlk->size))
#define OS_FSC_MEM_TAIL_MAGIC 0xABCDDCBA
#define OS_FSC_MEM_TAIL_SIZE (sizeof(uintptr_t))
#define OS_FSC_MEM_USED_HEAD_SIZE (sizeof(struct TagFscMemCtrl))

/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_GET_16BIT_HIGH_8BIT(num) ((num) >> 8)
#define OS_GET_32BIT_ARRAY_BASE(num) ((num) << 5)
#define OS_GET_32BIT_ARRAY_INDEX(num) ((num) >> 5)
#define OS_GET_32BIT_HIGH_16BIT(num) ((num) >> 16)
#define OS_GET_32BIT_HIGH_8BIT(num) (((num) >> 24) & 0xffU)
#define OS_GET_32BIT_LOW_16BIT(num) ((num) & 0xFFFFU)
#define OS_GET_32BIT_LOW_8BIT(num) ((num) & 0xffU)
#define OS_GET_64BIT_HIGH_32BIT(num) ((U32)((num) >> 32))
#define OS_GET_64BIT_LOW_32BIT(num) ((U32)((num) & 0xFFFFFFFFU))
#define OS_GET_8BIT_HIGH_4BIT(num) ((num) >> 4)
#define OS_GET_8BIT_LOW_4BIT(num) ((num) & 0xFU)

/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_IDLE_STACK_SIZE 512

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_INC_RDY_TSK_BIT_MAP(priority)

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_INIT_IDLE_TASK

/* From: src/include/uapi/prt_typedef.h */
#define OS_INT_LOCK() OsIntLock()
#define OS_INT_RESTORE(intSave) OsIntRestore(intSave)

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_INT_CNT_INC()
#define OS_INT_CNT_DEC()

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_MCMUTEX_LOCK(coreId, lock)
#define OS_MCMUTEX_UNLOCK(coreId, lock)

/* From: src/include/uapi/prt_sys.h */
#define OS_MAX_CORE_NUM 1

/* From: src/include/uapi/prt_task.h */
#define OS_MAX_TASK_IDX (OS_TSK_NUM_OF_PRIORITIES * OS_MAX_CORE_NUM)

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_MIN_RUNNING_TASK_NUM 1

/* From: src/include/uapi/prt_sem.h */
#define OS_MUTEX_INHERIT_MODE 0x02

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_NR_RUNNING_DEC(rq)
#define OS_NR_RUNNING_INC(rq)

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_PREEMPT_NOT_PENDING 0
#define OS_PREEMPT_PENDING 1

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_PRIO_HIGH_MASK 0x000000FFU
#define OS_PRIO_LOW_MASK 0xFFFFFF00U

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_QUEUE_PRIO_MULTI 0x00000001

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_RESET_RDY_TSK_BIT_MAP()

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_RESCHEDULE_ENABLE

/* From: src/include/uapi/prt_task.h */
#define OS_SIGNAL_MAX 32

/* From: src/include/uapi/prt_typedef.h */
#define OS_SPIN_LOCK(lock) PRT_SplLock(lock)
#define OS_SPIN_UNLOCK(lock, flags) PRT_SplUnlock(lock, flags)

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_STOP_IDLE_TASK()

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_SWITCH_RUNQ

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TASK_LOCK()
#define OS_TASK_UNLOCK()

/* From: src/include/uapi/prt_task.h */
#define OS_TSK_MIN_STACK_SIZE 256

/* From: src/include/uapi/prt_sem.h */
#define OS_TSK_NAME_LEN 16

/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_TSK_PRIORITY_COUNT 32

/* From: src/include/uapi/prt_task.h */
#define OS_TSK_STACK_SIZE_ALIGN 8

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_TCB_INDEX_BITS 8

/* From: src/include/uapi/prt_sem.h */
#define OS_UNLOCK_TASK()

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_UPDATE_HIGHEST_PRIORITY()

/* From: src/include/uapi/prt_typedef.h */
#define OS_UNUSED(x) ((void)(x))

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_USE_SMP

/* From: src/include/uapi/prt_typedef.h */
#define OS_VOLATILE volatile

/* From: src/core/kernel/include/prt_task_external.h */
#define OS_WAKE_UP_IDLE_TASK()

/* From: src/include/uapi/prt_sem.h */
#define PERF_DATA_MAGIC_WORD 0xA55AF00DU

/* From: src/include/uapi/prt_perf.h */
#define PERF_MAX_CALLCHAIN_DEPTH 16
#define PERF_MAX_EVENT 8
#define PERF_MAX_FILTER_TSKS 16

/* From: src/include/uapi/prt_perf.h */
#define PERF_PMU_STOPED 0U
#define PERF_PMU_STARTED 1U

/* From: src/include/uapi/prt_perf.h */
#define PERF_RECORD_CPU       (1U << 0)
#define PERF_RECORD_TID       (1U << 1)
#define PERF_RECORD_TYPE      (1U << 2)
#define PERF_RECORD_PERIOD    (1U << 3)
#define PERF_RECORD_TIMESTAMP (1U << 4)
#define PERF_RECORD_IP        (1U << 5)
#define PERF_RECORD_CALLCHAIN (1U << 6)

/* From: src/include/uapi/prt_perf.h */
#define PERF_STATUS_INIT 1U

/* From: src/include/uapi/prt_perf.h */
#define PERF_UNINIT 0U
#define PERF_STARTED 3U
#define PERF_STOPED 2U

/* From: src/include/uapi/prt_typedef.h */
#define PRT_ATOMIC_CAS(ptr, old, new) __sync_bool_compare_and_swap(ptr, old, new)

/* From: src/include/uapi/prt_atomic.h */
#define PRT_ATOMIC_LOAD(ptr) (*(ptr))

/* From: src/include/uapi/prt_atomic.h */
#define PRT_ATOMIC_STORE(ptr, val) (*(ptr) = (val))

/* From: src/include/uapi/prt_perf.h */
#define PRT_DL_LIST_ENTRY(ptr, type, member) COMPLEX_OF(ptr, type, member)

/* From: src/include/uapi/prt_typedef.h */
#define PRT_INLINE static __inline __attribute__((always_inline))

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_ADD(node, head) ListAdd(node, head)

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_DELETE(node) ListDel(node)

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_EMPTY(head) ListEmpty(head)

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_INIT(head) OS_LIST_INIT(head)

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_INSERT_AFTER(node, pos)

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_INSERT_BEFORE(node, pos)

/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_REPLACE(old, new)

/* From: src/include/uapi/prt_typedef.h */
#define PRT_NULL ((void *)0)

/* From: src/include/uapi/prt_atomic.h */
#define PRT_SPL_IRQ_LOCK(lock) PRT_SplIrqLock(lock)
#define PRT_SPL_IRQ_UNLOCK(lock, flags) PRT_SplIrqUnlock(lock, flags)

/* From: src/include/uapi/prt_atomic.h */
#define PRT_SPL_LOCK(lock) PRT_SplLock(lock)
#define PRT_SPL_UNLOCK(lock, flags) PRT_SplUnlock(lock, flags)

/* From: src/include/uapi/prt_perf.h */
#define PRT_SYNC_FULL_MEMORY_BARRIER() __sync_synchronize()

/* From: src/include/uapi/prt_perf.h */
#define PRT_SYNC_LOAD_MEMORY_BARRIER() __sync_synchronize()

/* From: src/include/uapi/prt_perf.h */
#define PRT_SYNC_STORE_MEMORY_BARRIER() __sync_synchronize()

/* From: src/include/uapi/prt_task.h */
#define SMP_COREID_FROM_PID(pid) ((U8)((pid) >> OS_TSK_TCB_INDEX_BITS))

/* From: src/include/uapi/prt_task.h */
#define SMP_GET_PID(coreId, taskId) (((U32)(coreId) << OS_TSK_TCB_INDEX_BITS) | (taskId))

/* From: src/core/kernel/include/prt_sched_external.h */
#define SMP_LOCK_RUNQ(coreId)

/* From: src/core/kernel/include/prt_sched_external.h */
#define SMP_UNLOCK_RUNQ(coreId)

/* From: src/core/kernel/include/prt_task_external.h */
#define SMP_UPDATE_CORE_IDLE(coreId)

/* From: src/include/uapi/prt_task.h */
#define TSK_GET_INDEX(pid) ((pid) & ((1U << OS_TSK_TCB_INDEX_BITS) - 1))

/* From: src/include/uapi/prt_task.h */
#define TSK_ID_INVALID 0xFFFFFFFFU

/* From: src/include/uapi/prt_task.h */
#define TSK_ID_TO_INDEX(pid) ((pid) & ((1U << OS_TSK_TCB_INDEX_BITS) - 1))

/* From: src/include/uapi/prt_task.h */
#define TSK_INDEX_TO_ID(index) (index)

/* From: src/include/uapi/prt_task.h */
#define TRUE 1

/* From: src/include/uapi/prt_typedef.h */
#define ULLONG_MAX 0xFFFFFFFFFFFFFFFFULL

/* From: src/include/uapi/prt_typedef.h */
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

/* From: src/include/uapi/prt_task.h */
#define USER_TASK_PRIORITY_LOWEST OS_TSK_PRIORITY_LOWEST

/* From: src/include/uapi/prt_task.h */
#define USER_TASK_PRIORITY_HIGHEST 0

/* From: src/include/uapi/prt_typedef.h */
#define VOLATILE volatile

/* From: src/core/kernel/include/prt_task_external.h */
#define WAIT_INTERRUPTIBLE 0x01

/* From: src/core/kernel/include/prt_task_external.h */
#define WAIT_UNINTERRUPTIBLE 0x00

#endif /* AUTO_STUB_PRT_PERF_H */
