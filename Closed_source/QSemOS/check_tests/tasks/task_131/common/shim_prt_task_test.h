#ifndef SHIM_PRT_TASK_TEST_H
#define SHIM_PRT_TASK_TEST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Include auto_stub.h to get struct definitions needed by inline functions below
#include "../auto_stub.h"

// Enable optional code paths for broader coverage
#ifndef OS_OPTION_POWEROFF
#define OS_OPTION_POWEROFF
#endif
#ifndef OS_OPTION_RR_SCHED
#define OS_OPTION_RR_SCHED
#endif
#ifndef OS_OPTION_SMP
#define OS_OPTION_SMP
#endif
#ifndef OS_MAX_CORE_NUM
#define OS_MAX_CORE_NUM 2
#endif

// Attribute and inline macros used in source
#define OS_SEC_TEXT
#define OS_SEC_L0_TEXT
#define OS_SEC_L2_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE

#ifndef LIKELY
#define LIKELY(x) (x)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) (x)
#endif

// Hook macros become no-ops in tests (only if not already defined)
#ifndef OS_MHOOK_ACTIVATE_PARA0
#define OS_MHOOK_ACTIVATE_PARA0(x) \
    do                             \
    {                              \
    } while (0)
#endif
#ifndef OS_SHOOK_ACTIVATE_PARA0
#define OS_SHOOK_ACTIVATE_PARA0(x) \
    do                             \
    {                              \
    } while (0)
#endif

// Basic types used in the source
typedef void (*OsVoidFunc)(void);
typedef void (*PowerOffFuncT)(void);
typedef void (*SetOfflineFlagFuncT)(void);

// Globals referenced by functions (provide test-visible definitions)
extern bool g_sysPowerOffFlag;
static inline void (*volatile g_dummy_ref__avoid_empty_units)(void) = 0;

// Power off hooks
PowerOffFuncT g_sysPowerOffHook = NULL;
SetOfflineFlagFuncT g_setOfflineFlagHook = NULL;

// Idle sleep hook
OsVoidFunc g_taskCoreSleep = NULL;

// Time slice global
unsigned int g_timeSliceCycle = 3;

// Ticks
uintptr_t g_uniTicks = 0;

// Missing macros that source code needs
// Note: OS_TSK_BLOCK in auto_stub.h is a broken multi-line macro, so we need to undef and redefine
#ifdef OS_TSK_BLOCK
#undef OS_TSK_BLOCK
#endif
#define OS_TSK_BLOCK 0x07U

// OS_THIS_CORE - provide a default value for single-core testing
#ifdef OS_THIS_CORE
#undef OS_THIS_CORE
#endif
#define OS_THIS_CORE 0

#ifdef FALSE
#undef FALSE
#endif
#ifndef FALSE
#define FALSE false
#endif

#ifdef TRUE
#undef TRUE
#endif
#ifndef TRUE
#define TRUE true
#endif

#ifdef OS_ALLCORES_MASK
#undef OS_ALLCORES_MASK
#endif
#define OS_ALLCORES_MASK 0xFFFFFFFFU

#ifndef OS_TSK_READY
#define OS_TSK_READY (1u << 0)
#endif

#ifndef OS_TSK_DELAY
#define OS_TSK_DELAY 0x0020U
#endif

#ifndef OS_TSK_PEND
#define OS_TSK_PEND 0x0008U
#endif

#ifndef OS_TSK_SUSPEND
#define OS_TSK_SUSPEND 0x0004U
#endif

#ifndef OS_TSK_QUEUE_PEND
#define OS_TSK_QUEUE_PEND 0x2000U
#endif

// OS_OK return code
#ifndef OS_OK
#define OS_OK 0U
#endif

// List manipulation functions needed by source
// auto_stub.h defines TagListObject with prev/next pointers
#ifndef ListEmpty
#define ListEmpty(l) ((l)->next == (l))
#endif

#ifndef ListDelete
#define ListDelete(node) do { \
    (node)->prev->next = (node)->next; \
    (node)->next->prev = (node)->prev; \
} while (0)
#endif

#ifndef ListTailAdd
#define ListTailAdd(node, list) do { \
    (node)->prev = (list)->prev; \
    (node)->next = (list); \
    (list)->prev->next = (node); \
    (list)->prev = (node); \
} while (0)
#endif

// TCB status macros - use taskStatus field which is defined in auto_stub.h's struct
// Note: auto_stub.h defines TagTskCb with taskStatus field
#ifndef TSK_STATUS_SET
#define TSK_STATUS_SET(task, flag) \
    do                             \
    {                              \
        (task)->taskStatus |= (flag); \
    } while (0)
#endif

#ifndef TSK_STATUS_CLEAR
#define TSK_STATUS_CLEAR(task, flag) \
    do                               \
    {                                \
        (task)->taskStatus &= ~(flag); \
    } while (0)
#endif

// Run queue globals - auto_stub.h has TagOsRunQue but not g_runq
// Provide g_runq as a static array and GET_RUNQ macro
#ifndef RUN_QUEUE_DEFINED
#define RUN_QUEUE_DEFINED

// Forward declaration to avoid conflict with auto_stub.h's definition
struct TagOsRunQue;

static struct TagOsRunQue g_runq[OS_MAX_CORE_NUM];

#ifndef GET_RUNQ
#define GET_RUNQ(coreID) (&g_runq[(coreID) % OS_MAX_CORE_NUM])
#endif
#endif

// Delay list global - auto_stub.h has TagOsTskSortedDelayList but not g_testDelayBase
#ifndef TEST_DELAY_BASE_DEFINED
#define TEST_DELAY_BASE_DEFINED

// Forward declaration
struct TagOsTskSortedDelayList;

static struct TagOsTskSortedDelayList g_testDelayBase;
#endif

// Instrumentation for assertions in tests
int g_calls_OsEnqueueTask = 0;
int g_calls_OsDequeueTask = 0;
int g_calls_OsIncNrRunning = 0;
int g_calls_OsDecNrRunning = 0;
int g_calls_OsReschedTask = 0;
int g_calls_OsReschedTaskNoWakeIpc = 0;
int g_last_sys_trace_pid = -1;
int g_calls_OsTskDlyNearestTicksRefresh = 0;

// Inline functions that use auto_stub.h's struct fields
// Note: These use taskStatus, isOnRq, nrRunning which exist in auto_stub.h's definitions

static inline void OsEnqueueTask(struct TagOsRunQue *rq, struct TagTskCb *task, int prio)
{
    (void)rq;
    (void)prio;
    g_calls_OsEnqueueTask++;
    task->isOnRq = true;
}

static inline void OsDequeueTask(struct TagOsRunQue *rq, struct TagTskCb *task, int prio)
{
    (void)rq;
    (void)prio;
    g_calls_OsDequeueTask++;
    task->isOnRq = false;
}

static inline void OsIncNrRunning(struct TagOsRunQue *rq)
{
    g_calls_OsIncNrRunning++;
    rq->nrRunning++;
}

static inline void OsDecNrRunning(struct TagOsRunQue *rq)
{
    g_calls_OsDecNrRunning++;
    rq->nrRunning--;
}

static inline void OsReschedTask(struct TagTskCb *task)
{
    (void)task;
    g_calls_OsReschedTask++;
}

static inline void OsReschedTaskNoWakeIpc(struct TagTskCb *task)
{
    (void)task;
    g_calls_OsReschedTaskNoWakeIpc++;
}

static inline void sys_trace_task_ready(int pid)
{
    g_last_sys_trace_pid = pid;
}

static inline void OsTskDlyNearestTicksRefresh(struct TagOsTskSortedDelayList *base)
{
    (void)base;
    g_calls_OsTskDlyNearestTicksRefresh++;
}

#ifndef CPU_OVERTIME_SORT_LIST_LOCK
#define CPU_OVERTIME_SORT_LIST_LOCK(base) \
    do                                    \
    {                                     \
        (void)(base);                     \
    } while (0)
#endif

#ifndef CPU_OVERTIME_SORT_LIST_UNLOCK
#define CPU_OVERTIME_SORT_LIST_UNLOCK(base) \
    do                                      \
    {                                       \
        (void)(base);                       \
    } while (0)
#endif

#ifndef OS_SET_DLYBASE_AND_TSK_CORE
#define OS_SET_DLYBASE_AND_TSK_CORE(tskDlyBase, taskCb) \
    do                                                  \
    {                                                   \
        (void)(taskCb);                                 \
        tskDlyBase = &g_testDelayBase;                  \
    } while (0)
#endif

// Helpers for tests
static inline void shim_reset_instrumentation(void)
{
    g_calls_OsEnqueueTask = 0;
    g_calls_OsDequeueTask = 0;
    g_calls_OsIncNrRunning = 0;
    g_calls_OsDecNrRunning = 0;
    g_calls_OsReschedTask = 0;
    g_calls_OsReschedTaskNoWakeIpc = 0;
    g_last_sys_trace_pid = -1;
    g_calls_OsTskDlyNearestTicksRefresh = 0;
    for (int i = 0; i < OS_MAX_CORE_NUM; ++i)
    {
        g_runq[i].nrRunning = 0;
        g_runq[i].tskCurr = NULL;
    }
    // Note: auto_stub.h's TagOsTskSortedDelayList has tskList field
    // But if we need ListInit, it should use auto_stub.h's ListInit
}

// OsTskReadyDelImpl - actual implementation called by auto_stub.h's OsTskReadyDel stub
extern inline void OsTskReadyDelImpl(struct TagTskCb *taskCb)
{
    struct TagOsRunQue *runQue = NULL;
    TSK_STATUS_CLEAR(taskCb, OS_TSK_READY);

    runQue = GET_RUNQ(taskCb->coreID);

    if (!(taskCb->isOnRq)) {
        return;
    }

    OsDequeueTask(runQue, taskCb, 0);
    OsDecNrRunning(runQue);

    if (taskCb == runQue->tskCurr) {
        OsReschedTask(taskCb);
    }

    return;
}

#endif // SHIM_PRT_TASK_TEST_H
