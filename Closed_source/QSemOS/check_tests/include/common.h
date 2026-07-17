#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/* Basic types to match project style */
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uintptr_t UINTPTR;

/* Return codes used by prt_queue.c (subset) */
#define OS_OK 0U
#define OS_ERRNO_QUEUE_INVALID 0x02001U
#define OS_ERRNO_QUEUE_PTR_NULL 0x02002U
#define OS_ERRNO_QUEUE_SIZE_ZERO 0x02003U
#define OS_ERRNO_QUEUE_NOT_CREATE 0x02004U
#define OS_ERRNO_QUEUE_SIZE_TOO_BIG 0x02005U
#define OS_ERRNO_QUEUE_NO_SOURCE 0x02006U
#define OS_ERRNO_QUEUE_IN_INTERRUPT 0x02007U
#define OS_ERRNO_QUEUE_PEND_IN_LOCK 0x02008U
#define OS_ERRNO_QUEUE_TIMEOUT 0x02009U
#define OS_ERRNO_QUEUE_PRIO_INVALID 0x0200AU

#define EOK 0

/* Bool compatibility macros used by original code */
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

/* Helpers used in prt_queue.c */
#define OS_SEC_ALW_INLINE
#define INLINE inline
#define OS_SEC_L4_TEXT

/* Interrupt/sched emulation */
extern volatile int g_os_int_active;
extern volatile int g_os_hwi_active;
#define OS_INT_ACTIVE (g_os_int_active != 0)
#define OS_HWI_ACTIVE (g_os_hwi_active != 0)

/* Task lock emulation */
extern volatile U32 OS_TASK_LOCK_DATA;

/* Fake PID composition */
#define OS_HWI_HANDLE 0xFFFFU
#define COMPOSE_PID(cpu, tsk) ((U16)(tsk))

/* Minimal queue constants/structs guessed from usage */
#define OS_QUEUE_NO_WAIT 0U
#define OS_QUEUE_WAIT_FOREVER 0xFFFFFFFFU

typedef enum
{
    OS_QUEUE_NORMAL = 0,
    OS_QUEUE_URGENT = 1
} QueuePrio;

/* Minimal list implementation for pend lists - only define if not already defined */
#ifndef TAG_LIST_OBJECT_DEFINED
#define TAG_LIST_OBJECT_DEFINED
struct TagListObject
{
    struct TagListObject *prev;
    struct TagListObject *next;
};
#endif

static inline void ListInit(struct TagListObject *l)
{
    l->next = l->prev = l;
}
static inline int ListEmpty(struct TagListObject *l)
{
    return l->next == l;
}
static inline void ListDelete(struct TagListObject *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}
static inline void ListTailAdd(struct TagListObject *node, struct TagListObject *l)
{
    node->prev = l->prev;
    node->next = l;
    l->prev->next = node;
    l->prev = node;
}
#define OS_LIST_FIRST(l) ((l)->next)

/* Forward decls */
struct TagTskCb;
struct TagQueCb;

/* Running task pointer simulation */
extern struct TagTskCb *g_running_task;
#define RUNNING_TASK (g_running_task)

/* Task status flags used */
#define OS_TSK_QUEUE_PEND 0x0001U
#define OS_TSK_TIMEOUT 0x0002U
#define OS_TSK_QUEUE_BUSY 0x0004U
#define OS_TSK_SUSPEND 0x0008U

#define TSK_STATUS_SET(tsk, flag) ((tsk)->taskStatus |= (flag))
#define TSK_STATUS_CLEAR(tsk, flag) ((tsk)->taskStatus &= ~(flag))

/* Timer detach stub */
#define OS_TSK_DELAY_LOCKED_DETACH(tsk) ((void)0)

/* Minimal task control block subset - only define if not already defined */
#ifndef TAG_TSK_CB_DEFINED
#define TAG_TSK_CB_DEFINED
struct TagTskCb
{
    U16 taskPid;
    U16 taskStatus;
    struct TagListObject pendList;
    void *taskPend;
};
#endif

/* Minimal queue node layout inferred */
struct QueNode
{
    U16 size;
    U16 srcPid;
    U8 buf[1]; /* flexible-ish */
};

/* Queue ID helper - must match real OS_QUEUE_INNER_ID */
#define OS_QUEUE_INNER_ID(queueId) ((queueId) - 1)

/* GET_QUEUE_HANDLE - get queue control block from inner ID */
#define GET_QUEUE_HANDLE(innerId) (&g_allQueue[(innerId)])

/* List component helper - get struct from list node */
#define LIST_COMPONENT(ptr, type, member) ((type *)(((char *)(ptr)) - offsetof(type, member)))

/* GET_TCB_PEND - get TCB from pend list node */
#define GET_TCB_PEND(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)

/* Queue control block subset */
struct TagQueCb
{
    U8 *queue;    /* raw buffer holding nodes */
    U16 nodeSize; /* bytes per node */
    U16 nodeNum;  /* number of nodes */
    U16 queueHead;
    U16 queueTail;
    U16 nodePeak;

    U16 readableCnt;
    U16 writableCnt;

    U16 queueState; /* 0=unused, 1=used */

    struct TagListObject readList;
    struct TagListObject writeList;
};

#define OS_QUEUE_UNUSED 0
#define OS_QUEUE_USED 1

/* Node header length = size + srcPid */
#define OS_QUEUE_NODE_HEAD_LEN ((U16)sizeof(U16) + (U16)sizeof(U16))

/* Lock macros — single-threaded stubs */
#define QUEUE_CB_IRQ_LOCK(qcb, save) ((void)(qcb), (void)(save))
#define QUEUE_CB_IRQ_UNLOCK(qcb, save) ((void)(qcb), (void)(save))
#define QUEUE_CB_LOCK(qcb) ((void)(qcb))
#define QUEUE_CB_UNLOCK(qcb) ((void)(qcb))

/* Spinlock stubs */
#ifndef OS_SPIN_LOCK_DEFINED
#define OS_SPIN_LOCK_DEFINED
static inline void OsSpinLockTaskRq(struct TagTskCb *t) { (void)t; }
static inline void OsSpinUnlockTaskRq(struct TagTskCb *t) { (void)t; }
#endif

/* Scheduler/timer stubs */
#ifndef OS_TSK_SCHED_DEFINED
#define OS_TSK_SCHED_DEFINED
void OsTskReadyDel(struct TagTskCb *t);
void OsTskReadyAddBgd(struct TagTskCb *t);
void OsTskTimerAdd(struct TagTskCb *t, U32 ticks);
void OsTskSchedule(void);
#endif

/* Error macro used in memcpy_s failure paths */
#define OS_GOTO_SYS_ERROR1() ((void)0)

/* minimal secure memcpy mimic */
static inline int memcpy_s(void *dest, size_t destsz, const void *src, size_t count)
{
    if (count > destsz)
        return -1;
    memcpy(dest, src, count);
    return EOK;
}

/* Additional constants/functions referenced */
#define OS_QUEUE_PID_INVALID 0xFFFFU
void OsIntRestore(uintptr_t save);

#endif /* COMMON_H */
