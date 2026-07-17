#ifndef SHIM_PRT_TASK_TEST_H
#define SHIM_PRT_TASK_TEST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Define guard macros before including auto_stub.h to prevent struct redefinitions
#ifndef TAG_TSK_CB_DEFINED
#define TAG_TSK_CB_DEFINED
#endif
#ifndef TAG_OS_RUN_QUE_DEFINED
#define TAG_OS_RUN_QUE_DEFINED
#endif
#ifndef TAG_OS_TSK_SORTED_DELAY_LIST_DEFINED
#define TAG_OS_TSK_SORTED_DELAY_LIST_DEFINED
#endif

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

// Hook macros become no-ops in tests
#define OS_MHOOK_ACTIVATE_PARA0(x) \
    do                             \
    {                              \
    } while (0)
#define OS_SHOOK_ACTIVATE_PARA0(x) \
    do                             \
    {                              \
    } while (0)

// Basic types used in the source
typedef void (*OsVoidFunc)(void);
typedef void (*PowerOffFuncT)(void);
typedef void (*SetOfflineFlagFuncT)(void);

// Globals referenced by functions (provide test-visible definitions)
extern bool g_sysPowerOffFlag; // defined by including prt_task.c
static inline void (*volatile g_dummy_ref__avoid_empty_units)(void) = 0;

// Power off hooks
extern PowerOffFuncT g_sysPowerOffHook;
extern SetOfflineFlagFuncT g_setOfflineFlagHook;

// Idle sleep hook
extern OsVoidFunc g_taskCoreSleep;

// Time slice global
extern unsigned int g_timeSliceCycle;

// Ticks
extern uintptr_t g_uniTicks;

// List node structure - already defined in auto_stub.h, don't redefine
// struct TagListObject is defined in auto_stub.h

static inline void ListInit(struct TagListObject *list)
{
    list->prev = list;
    list->next = list;
}

static inline bool ListEmpty(struct TagListObject *list)
{
    return list->next == list;
}

#ifndef container_of
#define container_of(ptr, type, member) ((type *)(((char *)(ptr)) - offsetof(type, member)))
#endif

static inline void __ListAddToList(struct TagListObject *node, struct TagListObject *list)
{
    // Insert node before list (as tail in circular list)
    node->next = list;
    node->prev = list->prev;
    list->prev->next = node;
    list->prev = node;
}

// Simple macro - both cases use the same insertion logic for circular list
#define ListTailAdd(node, tailOrList) __ListAddToList(node, tailOrList)

// Iterate over a list, collecting nodes by their timerList member
#define LIST_FOR_EACH(item, listPtr, type, member)                                                                             \
    for (struct TagListObject *__iter = (listPtr)->next, *__head = (listPtr);                                                   \
         __iter != __head && ((item = container_of(__iter, type, member)), 1);                                                  \
         __iter = __iter->next)

// Task control block - use same field names as auto_stub.h
// Skip definition if auto_stub.h already defined it
#ifndef TAG_TSK_CB_DEFINED
#define TAG_TSK_CB_DEFINED
struct TagTskCb
{
    int coreID;
    unsigned int taskStatus;
    unsigned int timeSlice;
    bool isOnRq;
    int taskPid;
    U64 expirationTick;
    struct TagListObject timerList;
};
#endif

#define OS_TSK_READY (1u << 0)
#define TSK_STATUS_SET(task, flag) \
    do                             \
    {                              \
        (task)->taskStatus |= (flag);  \
    } while (0)
#define TSK_STATUS_CLEAR(task, flag) \
    do                               \
    {                                \
        (task)->taskStatus &= ~(flag);   \
    } while (0)

// Run queue - only define if not already defined by auto_stub.h
#ifndef TAG_OS_RUN_QUE_DEFINED
#define TAG_OS_RUN_QUE_DEFINED
struct TagOsRunQue
{
    struct TagTskCb *tskCurr;
    int nrRunning;
};
#endif

#ifndef RUN_QUEUE_DEFINED
#define RUN_QUEUE_DEFINED
static struct TagOsRunQue g_runq[OS_MAX_CORE_NUM];
#endif

#define GET_RUNQ(coreID) (&g_runq[(coreID) % OS_MAX_CORE_NUM])

// Instrumentation for assertions in tests
extern int g_calls_OsEnqueueTask;
extern int g_calls_OsDequeueTask;
extern int g_calls_OsIncNrRunning;
extern int g_calls_OsDecNrRunning;
extern int g_calls_OsReschedTask;
extern int g_calls_OsReschedTaskNoWakeIpc;
extern int g_last_sys_trace_pid;
extern int g_calls_OsTskDlyNearestTicksRefresh;

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

// Delay list per-core base - only define if not already defined by auto_stub.h
#ifndef TAG_OS_TSK_SORTED_DELAY_LIST_DEFINED
#define TAG_OS_TSK_SORTED_DELAY_LIST_DEFINED
struct TagOsTskSortedDelayList
{
    struct TagListObject tskList;
};
#endif

#ifndef TEST_DELAY_BASE_DEFINED
#define TEST_DELAY_BASE_DEFINED
extern struct TagOsTskSortedDelayList *g_testDelayBase;
#endif

#define CPU_OVERTIME_SORT_LIST_LOCK(base) \
    do                                    \
    {                                     \
        (void)(base);                     \
    } while (0)
#define CPU_OVERTIME_SORT_LIST_UNLOCK(base) \
    do                                      \
    {                                       \
        (void)(base);                       \
    } while (0)

static inline void OsTskDlyNearestTicksRefresh(struct TagOsTskSortedDelayList *base)
{
    (void)base;
    g_calls_OsTskDlyNearestTicksRefresh++;
}

#define OS_SET_DLYBASE_AND_TSK_CORE(tskDlyBase, taskCb) \
    do                                                  \
    {                                                   \
        (void)(taskCb);                                 \
        tskDlyBase = g_testDelayBase;                   \
    } while (0)

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
    ListInit(&g_testDelayBase->tskList);
}

#endif // SHIM_PRT_TASK_TEST_H
