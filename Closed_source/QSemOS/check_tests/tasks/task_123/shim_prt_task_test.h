#ifndef SHIM_PRT_TASK_TEST_H
#define SHIM_PRT_TASK_TEST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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

// Power off hooks - declare extern, define in test_stubs.c
extern PowerOffFuncT g_sysPowerOffHook;
extern SetOfflineFlagFuncT g_setOfflineFlagHook;

// Idle sleep hook
extern OsVoidFunc g_taskCoreSleep;

// Time slice global
extern unsigned int g_timeSliceCycle;

// Ticks
extern uintptr_t g_uniTicks;

// Simple intrusive doubly linked list infra compatible with source usage
struct TagListNode
{
    struct TagListNode *prev;
    struct TagListNode *next;
};

struct TagListObject
{
    struct TagListNode head; // circular list head
};

static inline void ListInit(struct TagListObject *list)
{
    list->head.next = &list->head;
    list->head.prev = &list->head;
}

static inline bool ListEmpty(struct TagListObject *list)
{
    return list->head.next == &list->head;
}

#ifndef container_of
#define container_of(ptr, type, member) ((type *)(((char *)(ptr)) - offsetof(type, member)))
#endif

static inline void __ListAddBeforeNode(struct TagListNode *node, struct TagListNode *pos)
{
    // Insert node before pos
    node->next = pos;
    node->prev = pos->prev;
    pos->prev->next = node;
    pos->prev = node;
}

static inline void __ListAddTailToList(struct TagListNode *node, struct TagListObject *list)
{
    // Insert before head (as tail)
    struct TagListNode *head = &list->head;
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

// Overloaded-like macro using _Generic to accept either a list head or an existing node as the 2nd arg
#define ListTailAdd(node, tailOrList) _Generic((tailOrList), \
    struct TagListObject *: __ListAddTailToList,             \
    struct TagListNode *: __ListAddBeforeNode)(node, tailOrList)

#define LIST_FOR_EACH(item, listPtr, type, member)                                                                             \
    for (struct TagListNode *__iter = (listPtr)->head.next, *__head = &((listPtr)->head);                                      \
         (__iter == __head ? (item = container_of(__head, type, member), 0) : (item = container_of(__iter, type, member), 1)); \
         __iter = __iter->next)

// Task control block (minimal fields used in source)
struct TagTskCb
{
    int coreID;
    unsigned int status;
    unsigned int timeSlice;
    bool isOnRq;
    int taskPid;
    uintptr_t expirationTick;
    struct TagListNode timerList; // used by delay list
};

#define OS_TSK_READY (1u << 0)
#define TSK_STATUS_SET(task, flag) \
    do                             \
    {                              \
        (task)->status |= (flag);  \
    } while (0)
#define TSK_STATUS_CLEAR(task, flag) \
    do                               \
    {                                \
        (task)->status &= ~(flag);   \
    } while (0)

// Run queue
struct TagOsRunQue
{
    struct TagTskCb *tskCurr;
    int nrRunning;
};

// Run queue - extern so it's shared across translation units
extern struct TagOsRunQue g_runq[OS_MAX_CORE_NUM];

#define GET_RUNQ(coreID) (&g_runq[(coreID) % OS_MAX_CORE_NUM])

// Instrumentation for assertions in tests - declare extern, define in test_stubs.c
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

// Delay list per-core base
struct TagOsTskSortedDelayList
{
    struct TagListObject tskList;
};

// Delay list - extern so it's shared across translation units
extern struct TagOsTskSortedDelayList g_testDelayBase;

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
        tskDlyBase = &g_testDelayBase;                  \
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
    ListInit(&g_testDelayBase.tskList);
}

#endif // SHIM_PRT_TASK_TEST_H
