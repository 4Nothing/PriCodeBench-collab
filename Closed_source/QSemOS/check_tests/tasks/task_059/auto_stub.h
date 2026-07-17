/*
 * Auto-generated stub header for prt_notifier.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/utility/lib/prt_notifier.c
 * Included headers: 5
 */
#ifndef AUTO_STUB_PRT_NOTIFIER_H
#define AUTO_STUB_PRT_NOTIFIER_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char S8;
typedef signed short S16;
typedef signed int S32;
typedef signed long long S64;
typedef void *VirtAddr;
typedef void *PhyAddr;
/* ---- Types ---- */
/* From: src/include/uapi/prt_hwi.h */
typedef uintptr_t HwiArg;
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
/* From:  */
enum OsHwiIpiType {
    OS_TYPE_TRIGGER_BY_MASK = 0, /* 通过mask确定需要触发的目标核 */
    OS_TYPE_TRIGGER_TO_OTHER, /* 触发除本核外的其他核 */
    OS_TYPE_TRIGGER_TO_SELF, /* 触发本核 */
    OS_TYPE_TRIGGER_BUTT /* 非法 */
};
enum MoudleId {
    OS_MID_SYS = 0x0, /* 系统模块 */
    OS_MID_MEM = 0x1, /* 内存模块 */
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
/* From: src/include/uapi/prt_hwi.h */
typedef void (*HwiProcFunc)(HwiArg);
typedef U16 HwiMode;
typedef U16 HwiPrior;
typedef U32 HwiHandle;
typedef void (*HwiExitHook)(U32 hwiNum);
typedef void (*HwiEntryHook)(U32 hwiNum);
/* From: src/utility/lib/include/prt_notifier.h */
struct NotifierBlock;  /* Forward declaration */
typedef int (*NotifierFn)(struct NotifierBlock *nb,
            int action, void *data);
struct NotifierBlock {
    NotifierFn call;
    struct NotifierBlock *next;
    int priority;
};
struct NotifierHead {
    struct NotifierBlock *head;
};
/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))
/* From: src/include/uapi/prt_errno.h */
#define ERRNO_OS_ID (0x00U << 16)
#define ERRTYPE_ERROR (0x02U << 24)
#define ERRTYPE_FATAL (0x03U << 24)
#define ERRTYPE_NORMAL (0x00U << 24)
#define ERRTYPE_WARN (0x01U << 24)
/* From: src/include/uapi/prt_typedef.h */
#define FALSE ((bool)0)
#define INLINE static __inline __attribute__((always_inline))
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define NO 0
/* From: src/utility/lib/include/prt_notifier.h */
#define NOTIFY_BAD (NOTIFY_STOP_MASK|0x0002)
#define NOTIFY_DONE 0x0000      /* Don't care */
#define NOTIFY_OK 0x0001      /* Suits me */
#define NOTIFY_STOP (NOTIFY_OK|NOTIFY_STOP_MASK)
#define NOTIFY_STOP_MASK 0x8000      /* Don't call further */
/* From: src/include/uapi/prt_typedef.h */
#define NULL ((void *)0)
#define OS_EMBED_ASM __asm__ __volatile__
/* From: src/include/uapi/prt_errno.h */
#define OS_ERRNO_BUILD_ERROR(mid, errno) (ERRTYPE_ERROR | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
#define OS_ERRNO_BUILD_FATAL(mid, errno) (ERRTYPE_FATAL | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
/* From: src/include/uapi/prt_hwi.h */
#define OS_ERRNO_HWI_AFFINITY_HWINUM_SGI OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x18)
#define OS_ERRNO_HWI_AFFINITY_MASK_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x17)
#define OS_ERRNO_HWI_ALREADY_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x03)
#define OS_ERRNO_HWI_ATTR_CONFLICTED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x06)
#define OS_ERRNO_HWI_COMBINEHOOK_ALREADY_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x08)
#define OS_ERRNO_HWI_CORE_ID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0c)
#define OS_ERRNO_HWI_DELETED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0a)
#define OS_ERRNO_HWI_DELETE_INT OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x11)
#define OS_ERRNO_HWI_HW_REPORT_HWINO_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0d)
#define OS_ERRNO_HWI_MEMORY_ALLOC_FAILED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x07)
#define OS_ERRNO_HWI_MODE_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x09)
#define OS_ERRNO_HWI_MODE_UNSET OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0b)
#define OS_ERRNO_HWI_NUM_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x01)
#define OS_ERRNO_HWI_PRI_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x02)
#define OS_ERRNO_HWI_PROC_FUNC_NULL OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x04)
#define OS_ERRNO_HWI_RESOURCE_ALLOC_FAILED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0f)
#define OS_ERRNO_HWI_TRIGGER_HWINUM_NOT_SGI OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x14)
#define OS_ERRNO_HWI_TRIGGER_MASK_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x16)
#define OS_ERRNO_HWI_TRIGGER_TYPE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x15)
#define OS_ERRNO_HWI_UNCREATED OS_ERRNO_BUILD_FATAL(OS_MID_HWI, 0x05)
#define OS_ERRNO_MULTI_TARGET_CORE OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x10)
/* From: src/include/uapi/prt_typedef.h */
#define OS_ERROR (U32)(-1)
/* From: src/include/uapi/prt_hwi.h */
#define OS_ERROR_HWI_BASE_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x12)
#define OS_ERROR_HWI_INT_LOCK_REG_PARA_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x13)
#define OS_ERROR_HWI_INT_REGISTER_FAILED OS_ERRNO_BUILD_FATAL(OS_MID_HWI, 0x0e)
/* From: src/include/uapi/prt_typedef.h */
#define OS_FAIL 1
/* From: src/include/uapi/prt_hwi.h */
#define OS_HWI_ATTR(mode, type) (HwiMode)((HwiMode)(mode) | (HwiMode)(type))
#define OS_HWI_IPI_NO_00 0
#define OS_HWI_IPI_NO_01 1
#define OS_HWI_IPI_NO_011 11
#define OS_HWI_IPI_NO_012 12
#define OS_HWI_IPI_NO_013 13
#define OS_HWI_IPI_NO_014 14
#define OS_HWI_IPI_NO_015 15
#define OS_HWI_IPI_NO_02 2
#define OS_HWI_IPI_NO_03 3
#define OS_HWI_IPI_NO_04 4
#define OS_HWI_IPI_NO_05 5
#define OS_HWI_IPI_NO_06 6
#define OS_HWI_IPI_NO_07 7
#define OS_HWI_IPI_NO_08 8
#define OS_HWI_IPI_NO_09 9
#define OS_HWI_IPI_NO_10 10
#define OS_HWI_MODE_COMBINE 0x8000
#define OS_HWI_MODE_DEFAULT OS_HWI_MODE_ENGROSS
#define OS_HWI_MODE_ENGROSS 0x4000
#define OS_HWI_TYPE_NORMAL 0x00
/* From: src/include/uapi/prt_typedef.h */
#define OS_INVALID (-1)
#define OS_OK 0
/* From: (build-generated) */
#define OS_SEC_ALIGN
#define OS_SEC_ALW_INLINE
#define OS_SEC_BSS
#define OS_SEC_DATA
#define OS_SEC_L0_TEXT
#define OS_SEC_L1_TEXT
#define OS_SEC_L2_TEXT
#define OS_SEC_L3_TEXT
#define OS_SEC_L4_BSS
#define OS_SEC_L4_DATA
#define OS_SEC_L4_TEXT
#define OS_SEC_RO
#define OS_SEC_RW
#define OS_SEC_TEXT
#define OS_SEC_WEAK
#define OS_SEC_ZI
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/include/uapi/prt_hwi.h */
#define PRT_HWI_H
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
#define TRUE ((bool)1)
#define TRUNCATE(addr, size) ((addr) & ~((uintptr_t)(size) - 1))
#define U12_INVALID 0xfffU
#define U16_INVALID 0xffffU
#define U32_INVALID 0xffffffffU
#define U32_MAX 0xFFFFFFFFU
#define U64_INVALID 0xffffffffffffffffUL
#define U8_INVALID 0xffU
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define YES 1
/* ---- Common runtime shims ---- */
/* PRT_HwiLock/Restore stubs — track lock nesting count */
extern volatile int g_hwi_lock_count;
static inline uintptr_t PRT_HwiLock(void) {
    g_hwi_lock_count++;
    return (uintptr_t)g_hwi_lock_count;
}
static inline void PRT_HwiRestore(uintptr_t flags) {
    (void)flags;
    g_hwi_lock_count--;
}
/* ---- Plain (non-Os-prefixed) notifier chain functions ---- */

/* NotifierCallChain now from source */
static inline int NotifierCallChain(struct NotifierBlock **nl, int val, void *v, int nr_to_call, int *nr_calls)
{
    int ret = NOTIFY_DONE;
    struct NotifierBlock *nb = *nl;
    while (nb && nr_to_call) {
        ret = nb->call(nb, val, v);
        if (nr_calls) (*nr_calls)++;
        if (ret & NOTIFY_STOP_MASK) break;
        nb = nb->next;
        nr_to_call--;
    }
    return ret;
}

static inline int NotifierChainRegister(struct NotifierBlock **nl, struct NotifierBlock *n)
{
    while ((*nl) != NULL) {
        if (*nl == n) return -1;
        if (n->priority > (*nl)->priority) break;
        nl = &((*nl)->next);
    }
    n->next = *nl;
    *nl = n;
    return 0;
}

static inline int NotifierChainUnregister(struct NotifierBlock **nl, struct NotifierBlock *n)
{
    while (*nl != NULL) {
        if (*nl == n) { *nl = n->next; return 0; }
        nl = &((*nl)->next);
    }
    return -1;
}

/* ---- Os-prefixed notifier functions ---- */

#ifndef SKIP_OsNotifierChainRegister
/* OsNotifierChainRegister - 使用NotifierHead结构注册 (优先级顺序插入) */
static inline int OsNotifierChainRegister(struct NotifierHead *nh, struct NotifierBlock *nb)
{
    struct NotifierBlock *p = nh->head;
    while (p) { if (p == nb) return -1; p = p->next; }
    if (nh->head == NULL || nh->head->priority <= nb->priority) {
        nb->next = nh->head; nh->head = nb;
    } else {
        struct NotifierBlock *curr = nh->head;
        while (curr->next && curr->next->priority > nb->priority) curr = curr->next;
        nb->next = curr->next; curr->next = nb;
    }
    return 0;
}
#endif

#ifndef SKIP_OsNotifierChainUnregister
/* OsNotifierChainUnregister - 使用NotifierHead结构注销 */
static inline int OsNotifierChainUnregister(struct NotifierHead *nh, struct NotifierBlock *nb)
{
    if (nh->head == NULL) return -1;
    if (nh->head == nb) { nh->head = nb->next; return 0; }
    struct NotifierBlock *curr = nh->head;
    while (curr->next) {
        if (curr->next == nb) { curr->next = nb->next; return 0; }
        curr = curr->next;
    }
    return -1;
}
#endif

#ifndef SKIP_OsNotifierRawCallChain
/* OsNotifierRawCallChain - 直接调用notifier chain不获取锁 */
static inline int OsNotifierRawCallChain(struct NotifierHead *nh, int action, void *data, int nr_to_call, int *nr_calls)
{
    int ret = NOTIFY_DONE;
    int calls = 0;
    struct NotifierBlock *nb = nh->head;
    int maxCalls = (nr_to_call < 0) ? 1000 : nr_to_call;
    while (nb && calls < maxCalls) {
        ret = nb->call(nb, action, data);
        calls++;
        if (ret & NOTIFY_STOP_MASK) break;
        nb = nb->next;
    }
    if (nr_calls) *nr_calls = calls;
    return ret;
}
#endif

/* ---- Die-notifier globals and stubs ---- */

static struct NotifierHead g_dieNotifier;

#if !defined(SKIP_OsRegisterDieNotifier) && !defined(SKIP_OsNotifierChainRegister)
static inline int OsRegisterDieNotifier(struct NotifierBlock *nb) {
    return OsNotifierChainRegister(&g_dieNotifier, nb);
}
#endif

#if !defined(SKIP_OsUnregisterDieNotifier) && !defined(SKIP_OsNotifierChainUnregister)
static inline int OsUnregisterDieNotifier(struct NotifierBlock *nb) {
    return OsNotifierChainUnregister(&g_dieNotifier, nb);
}
#endif

#ifndef SKIP_OsNotifyDie
static inline int OsNotifyDie(int val, void *v) {
    return NotifierCallChain(&g_dieNotifier.head, val, v, -1, NULL);
}
#endif

/* OsNotifierCallChain - 带锁调用的notifier chain */
#ifndef SKIP_OsNotifierCallChain
static inline int OsNotifierCallChain(struct NotifierHead *nh, int val, void *v, int nr_to_call, int *nr_calls)
{
    uintptr_t flags;
    int ret;
    flags = PRT_HwiLock();
    ret = NotifierCallChain(&nh->head, val, v, -1, NULL);
    PRT_HwiRestore(flags);
    (void)nr_to_call; (void)nr_calls;
    return ret;
}
#endif

#endif /* AUTO_STUB_PRT_NOTIFIER_H */
