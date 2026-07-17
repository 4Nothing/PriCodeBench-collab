#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef uint32_t U32;
typedef uint32_t TskHandle;
typedef uint32_t TskPrior;
#ifndef _UINTPTR_T_DEFINED
typedef unsigned long uintptr_t;
#endif

#ifndef OS_SEC_L4_TEXT
#define OS_SEC_L4_TEXT
#endif
#ifndef OS_SEC_ALW_INLINE
#define OS_SEC_ALW_INLINE static
#endif
#ifndef INLINE
#define INLINE inline
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define OS_OK ((U32)0)
#define OS_ERRNO_TSK_ID_INVALID ((U32)0x1001)
#define OS_ERRNO_TSK_PTR_NULL ((U32)0x1002)
#define OS_ERRNO_TSK_NOT_CREATED ((U32)0x1003)
#define OS_ERRNO_TSK_PRIOR_ERROR ((U32)0x1004)
#define OS_ERRNO_TSK_OPERATE_IDLE ((U32)0x1005)

#define OS_TSK_PRIORITY_LOWEST ((TskPrior)63u)

#define OS_TSK_READY (0x1u)
#define OS_TSK_UNUSED_FLAG (0x00000000u)

struct ListNode
{
    struct ListNode *prev;
    struct ListNode *next;
};

static inline void ListInit(struct ListNode *node)
{
    node->prev = node;
    node->next = node;
}

static inline bool ListEmpty(const struct ListNode *node)
{
    return node->next == node;
}

struct TagTskCb
{
    U32 taskStatus;
    TskPrior priority;
    TskPrior origPriority;
    struct ListNode semBList;
};

extern U32 g_tskMaxNum;
extern struct TagTskCb g_mockTcbs[];

/* Stub functions */
static inline int CHECK_TSK_PID_OVERFLOW(U32 pid) { return pid >= 0x10000000u; }
static inline struct TagTskCb *GET_TCB_HANDLE(U32 pid) { return &g_mockTcbs[pid]; }
static inline int OsIntLock(void) { return 0; }
static inline void OsIntRestore(int flags) { (void)flags; }
static inline int TSK_IS_UNUSED(struct TagTskCb *tcb) { return (tcb->taskStatus == 0) || ((tcb->taskStatus & OS_TSK_UNUSED_FLAG) != 0); }
static inline void OsSpinLockTaskRq(struct TagTskCb *tcb) { (void)tcb; }
static inline void OsSpinUnlockTaskRq(struct TagTskCb *tcb) { (void)tcb; }
static inline void OsTskReadyDel(struct TagTskCb *tcb) { (void)tcb; }
static inline void OsTskReadyAdd(struct TagTskCb *tcb) { (void)tcb; }
static inline void OsTskSchedule(void) { }
static inline U32 OsTaskPrioritySetCheck(U32 taskPid, TskPrior taskPrio)
{
    if (taskPrio > OS_TSK_PRIORITY_LOWEST) return OS_ERRNO_TSK_PRIOR_ERROR;
    if (CHECK_TSK_PID_OVERFLOW(taskPid)) return OS_ERRNO_TSK_ID_INVALID;
    if (taskPid >= g_tskMaxNum) return OS_ERRNO_TSK_OPERATE_IDLE;
    return OS_OK;
}
static inline U32 OsSemPrioBListLock(struct TagTskCb *taskCb) { (void)taskCb; return 0; }
static inline void OsSemPrioBListUnLock(U32 flag) { (void)flag; }

struct TestStats
{
    int total;
    int pass;
};

static inline void report_case_u32(const char *name, U32 expected, U32 actual, struct TestStats *stats)
{
    stats->total++;
    bool ok = (expected == actual);
    if (ok) stats->pass++;
    printf("[CASE] %s\n  Expected: %u\n  Actual  : %u\n  Result  : %s\n\n", name, (unsigned)expected, (unsigned)actual, ok ? "PASS" : "FAIL");
}

static inline void report_rate(const struct TestStats *stats)
{
    double rate = (stats->total == 0) ? 0.0 : (100.0 * (double)stats->pass / (double)stats->total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, stats->pass, stats->total);
}

#endif
