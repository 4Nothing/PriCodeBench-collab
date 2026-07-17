#pragma once
#include "os_types.h"

typedef struct TagListObject
{
    struct TagListObject *prev;
    struct TagListObject *next;
} TagListObject;

static INLINE void ListInit(TagListObject *head)
{
    head->prev = head->next = head;
}
static INLINE bool ListEmpty(const TagListObject *head)
{
    return head->next == head;
}
static INLINE void __list_add(TagListObject *newn, TagListObject *prev, TagListObject *next)
{
    next->prev = newn;
    newn->next = next;
    newn->prev = prev;
    prev->next = newn;
}
static INLINE void ListTailAdd(TagListObject *node, TagListObject *head)
{
    __list_add(node, head->prev, head);
}
static INLINE void ListDelete(TagListObject *entry)
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->prev = entry->next = entry;
}

#define LIST_FOR_EACH(pos, head, type, member)                          \
    for (pos = (type *)((char *)(head)->next - offsetof(type, member)); \
         &pos->member != (head);                                        \
         pos = (type *)((char *)pos->member.next - offsetof(type, member)))

#define OS_LIST_FIRST(head) ((head)->next)

typedef struct TagTskCb
{
    TagListObject pendList; // for waiting list
    TagListObject semBList; // list of mutexes held
    U32 taskPid;
    TskPrior priority;
    TskPrior origPriority;
    void *taskPend; // pend object
    U32 status;
} TagTskCb;

typedef struct TagSemCb
{
    TagListObject semList;  // waiters
    TagListObject semBList; // link in owner's list
    U32 semCount;
    U32 semType;
    U32 semMode;
    U32 semStat;
    U32 semOwner;
    U32 recurCount;
} TagSemCb;

extern TagTskCb g_dummyTask;
extern TagSemCb g_semPool[8];

/* Mutable interrupt/lock flags */
extern U32 g_int_active;
extern U32 g_task_lock_data;
extern U16 g_maxSem;

/* Instrumentation counters */
extern U32 g_ready_del_count;
extern U32 g_ready_add_count;
extern U32 g_ready_add_bgd_count;
extern U32 g_schedule_count;
extern U32 g_timer_add_count;

// Macros and constants to satisfy code
#define OS_SEM_UNUSED 0
#define OS_SEM_USED 1
#define OS_SEM_COUNT_MAX 0xffffffffu
#define OS_SEM_FULL 1u
#define OS_INVALID_OWNER_ID 0xffffffffu

#define SEM_TYPE_COUNT 0
#define SEM_TYPE_BIN 1
#define GET_SEM_TYPE(x) ((x) & 0x1)
#define MAKE_SEM_TYPE(t, m) (((t) & 0x1) | (((m) & 0x1) << 1))
#define GET_SEM(handle) ((handle) < g_maxSem ? &g_semPool[(handle)] : NULL)

#define SEM_MUTEX_TYPE_NORMAL 0
#define SEM_MUTEX_TYPE_RECUR 1
#define GET_MUTEX_TYPE(x) (((x) >> 1) & 0x1)

#define SEM_MODE_FIFO 0
#define SEM_MODE_PRIOR 1

#define OS_OK 0u
#define OS_WAIT_FOREVER 0xffffffffu

#define OS_ERRNO_SEM_INVALID 1u
#define OS_ERRNO_SEM_OVERFLOW 2u
#define OS_ERRNO_SEM_MUTEX_POST_INTERR 3u
#define OS_ERRNO_SEM_MUTEX_NOT_OWNER_POST 4u
#define OS_ERRNO_TSK_PRIORITY_INHERIT 5u
#define OS_ERRNO_TSK_PEND_MUTEX 6u
#define OS_ERRNO_TSK_PEND_PRIOR 7u
#define OS_ERRNO_TSK_PRIOR_LOWER_THAN_PENDTSK 8u
#define OS_ERRNO_SEM_UNAVAILABLE 9u
#define OS_ERRNO_SEM_PEND_IN_LOCK 10u
#define OS_ERRNO_SEM_PEND_INTERR 11u
#define OS_ERRNO_SEM_TIMEOUT 12u

#define OS_TSK_READY (1u << 0)
#define OS_TSK_PEND (1u << 1)
#define OS_TSK_SUSPEND (1u << 2)
#define OS_TSK_TIMEOUT (1u << 3)
#define OS_TSK_SUSPEND_READY_BLOCK (OS_TSK_SUSPEND)

#define TSK_STATUS_TST(t, mask) (((t)->status & (mask)) != 0)
#define TSK_STATUS_SET(t, mask) ((t)->status |= (mask))
#define TSK_STATUS_CLEAR(t, mask) ((t)->status &= ~(mask))

#define GET_TCB_PEND(node) ((TagTskCb *)((char *)(node) - offsetof(TagTskCb, pendList)))
#define GET_TCB_HANDLE(pid) (&g_dummyTask)

// Scheduler and lock stubs
typedef struct TagOsRunQue
{
    int dummy;
} TagOsRunQue;

/* Declarations only; tests provide definitions in stubs.c */
TagOsRunQue *OsSpinLockRunTaskRq(void);
void OsSpinUnLockRunTaskRq(TagOsRunQue *q);
void OsSpinLockTaskRq(TagTskCb *t);
void OsSpinUnlockTaskRq(TagTskCb *t);

void OsTskReadyDel(TagTskCb *t);
void OsTskReadyAdd(TagTskCb *t);
void OsTskReadyAddBgd(TagTskCb *t);
void OsTskTimerAdd(TagTskCb *t, U32 to);
static INLINE void OS_TSK_DELAY_LOCKED_DETACH(TagTskCb *t) { (void)t; }

void OsTskSchedule(void);
void OsTskScheduleFastPs(uintptr_t x);

// Interrupt/context flags
static INLINE void OsIntRestore(uintptr_t x) { (void)x; }
#define OS_INT_ACTIVE g_int_active
#define OS_TASK_LOCK_DATA g_task_lock_data

// Semaphore locks (no-op for standalone)
static INLINE void OsSemIfPrioLock(struct TagSemCb *s) { (void)s; }
static INLINE void OsSemIfPrioUnLock(struct TagSemCb *s) { (void)s; }

#define SEM_CB_IRQ_LOCK(sem, intSave) \
    do                                \
    {                                 \
        (void)(sem);                  \
        (intSave) = 0;                \
    } while (0)
#define SEM_CB_IRQ_UNLOCK(sem, intSave) \
    do                                  \
    {                                   \
        (void)(sem);                    \
        (void)(intSave);                \
    } while (0)
#define SEM_CB_UNLOCK(sem) \
    do                     \
    {                      \
        (void)(sem);       \
    } while (0)

// Perf macro stub
#define PRT_PERF(x) \
    do              \
    {               \
        (void)(x);  \
    } while (0)

// Running task accessor
extern TagTskCb *RUNNING_TASK;