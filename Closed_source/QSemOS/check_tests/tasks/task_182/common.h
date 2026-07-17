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
#ifndef OS_OK
#define OS_OK 0U
#endif

#define EOK 0

/* Bool compatibility macros used by original code */
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

/* Helpers used in prt_queue.c */
#ifndef OS_SEC_ALW_INLINE
#define OS_SEC_ALW_INLINE
#endif
#ifndef INLINE
#define INLINE inline
#endif
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
#ifndef OS_QUEUE_NO_WAIT
#define OS_QUEUE_NO_WAIT 0U
#endif
#ifndef OS_QUEUE_WAIT_FOREVER
#define OS_QUEUE_WAIT_FOREVER 0xFFFFFFFFU
#endif

typedef enum
{
    OS_QUEUE_NORMAL = 0,
    OS_QUEUE_URGENT = 1
} QueuePrio;

/* Minimal list implementation for pend lists */
struct TagListObject
{
    struct TagListObject *prev;
    struct TagListObject *next;
};

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

/* Forward decls - actual definitions come from mocks/prt_queue_external.h */
struct TagTskCb;
struct TagQueCb;
struct QueNode;

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
#define OS_TSK_DELAY_LOCKED_DETACH(tsk) ((void)(tsk))

/* Minimal task control block subset */
struct TagTskCb
{
    U16 taskPid;
    U16 taskStatus;
    struct TagListObject pendList;
    void *taskPend;
};

/* Queue ID helper - must match real OS_QUEUE_INNER_ID */
#ifndef OS_QUEUE_INNER_ID
#define OS_QUEUE_INNER_ID(queueId) ((queueId) - 1)
#endif

/* GET_QUEUE_HANDLE - get queue control block from inner ID */
#define GET_QUEUE_HANDLE(innerId) (&g_allQueue[(innerId)])

/* List component helper - get struct from list node */
#define LIST_COMPONENT(ptr, type, member) ((type *)(((char *)(ptr)) - offsetof(type, member)))

/* GET_TCB_PEND - get TCB from pend list node */
#define GET_TCB_PEND(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)

/* Lock macros — single-threaded stubs */
#define QUEUE_CB_IRQ_LOCK(qcb, save) ((void)(qcb), (void)(save))
#define QUEUE_CB_IRQ_UNLOCK(qcb, save) ((void)(qcb), (void)(save))
#define QUEUE_CB_LOCK(qcb) ((void)(qcb))
#define QUEUE_CB_UNLOCK(qcb) ((void)(qcb))

/* Spinlock stubs */
static inline void OsSpinLockTaskRq(struct TagTskCb *t) { (void)t; }
static inline void OsSpinUnlockTaskRq(struct TagTskCb *t) { (void)t; }

/* Scheduler/timer stubs */
void OsTskReadyDel(struct TagTskCb *t);
void OsTskReadyAddBgd(struct TagTskCb *t);
void OsTskTimerAdd(struct TagTskCb *t, U32 ticks);
void OsTskSchedule(void);

/* Error codes - actual definitions may come from mocks */
#ifndef OS_ERRNO_QUEUE_MAXNUM_ZERO
#define OS_ERRNO_QUEUE_MAXNUM_ZERO 0x1001U
#endif
#ifndef OS_ERRNO_QUEUE_NO_MEMORY
#define OS_ERRNO_QUEUE_NO_MEMORY 0x1002U
#endif
#ifndef OS_ERRNO_QUEUE_CREAT_PTR_NULL
#define OS_ERRNO_QUEUE_CREAT_PTR_NULL 0x1003U
#endif
#ifndef OS_ERRNO_QUEUE_PARA_ZERO
#define OS_ERRNO_QUEUE_PARA_ZERO 0x1004U
#endif
#ifndef OS_ERRNO_QUEUE_NSIZE_INVALID
#define OS_ERRNO_QUEUE_NSIZE_INVALID 0x1005U
#endif
#ifndef OS_ERRNO_QUEUE_CB_UNAVAILABLE
#define OS_ERRNO_QUEUE_CB_UNAVAILABLE 0x1006U
#endif
#ifndef OS_ERRNO_QUEUE_CREATE_NO_MEMORY
#define OS_ERRNO_QUEUE_CREATE_NO_MEMORY 0x1007U
#endif
#ifndef OS_ERRNO_QUEUE_NO_SOURCE
#define OS_ERRNO_QUEUE_NO_SOURCE 0x1008U
#endif
#ifndef OS_ERRNO_QUEUE_IN_INTERRUPT
#define OS_ERRNO_QUEUE_IN_INTERRUPT 0x1009U
#endif
#ifndef OS_ERRNO_QUEUE_PEND_IN_LOCK
#define OS_ERRNO_QUEUE_PEND_IN_LOCK 0x100AU
#endif
#ifndef OS_ERRNO_QUEUE_TIMEOUT
#define OS_ERRNO_QUEUE_TIMEOUT 0x100BU
#endif

/* Error macro used in memcpy_s failure paths */
#ifndef OS_GOTO_SYS_ERROR1
#define OS_GOTO_SYS_ERROR1() ((void)0)
#endif

/* minimal secure memcpy mimic */
static inline int memcpy_s(void *dest, size_t destsz, const void *src, size_t count)
{
    if (count > destsz)
        return -1;
    memcpy(dest, src, count);
    return EOK;
}

/* Additional constants/functions referenced */
#ifndef OS_QUEUE_PID_INVALID
#define OS_QUEUE_PID_INVALID 0xFFFFU
#endif
/* Note: OsIntRestore is defined in mocks/prt_queue_external.h */

#endif /* COMMON_H */