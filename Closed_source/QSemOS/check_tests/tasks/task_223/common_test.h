// Common test harness for prt_task_lock.c unit tests
#ifndef COMMON_TEST_H
#define COMMON_TEST_H

#define OsTaskIrqUnlock
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef uint32_t U32;
typedef uint32_t TskHandle;
typedef uint32_t TskPrior;

enum
{
    OS_OK = 0,
    OS_ERRNO_TSK_YIELD_INVALID_TASK = 1,
    OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK = 2,
    OS_ERRNO_TSK_DELAY_IN_INT = 3,
    OS_ERRNO_TSK_DELAY_IN_LOCK = 4,
    OS_ERRNO_TSK_PRIOR_ERROR = 5,
    OS_ERRNO_BUILD_ID_INVALID = 6,
    OS_ERRNO_TSK_UNLOCK_NO_LOCK = 7,
};

#define OS_TSK_READY (1u << 0)
#define OS_TSK_RUNNING (1u << 1)
#define OS_TSK_DELAY (1u << 2)

#define OS_TSK_PRIORITY_LOWEST 31u

#define OS_HWI_ACTIVE_MASK 0x1u

struct TagListObject
{
    struct TagListObject *prev, *next;
};

struct TagTskCb
{
    TskHandle taskPid;
    U32 priority;
    U32 status;
    struct TagListObject pendList;
};

struct TagOsRunQue
{
    U32 uniTaskLock;
    U32 needReschedule;
    U32 uniFlag;
};

extern volatile U32 OS_INT_ACTIVE;
extern volatile U32 OS_TASK_LOCK_DATA;
extern struct TagOsRunQue g_rq;
extern struct TagTskCb *g_running_task;

extern int g_schedule_fast_called;
extern int g_schedule_fast_ps_called;
extern int g_smp_schedule_trigger_called;
extern int g_spin_lock_called;
extern int g_spin_unlock_called;
extern int g_ready_del_called;
extern int g_timer_add_called;

extern U32 g_next_intsave_value;
extern int g_int_lock_called;
extern int g_int_restore_called;
extern U32 g_last_intsave_returned;
extern U32 g_last_intrestore_arg;

extern int g_di_state_check_return;

#define PRIORITY_MAX 32
extern struct TagListObject g_ready_queues[PRIORITY_MAX];

void ListInit(struct TagListObject *head);
void ListAdd(struct TagListObject *node, struct TagListObject *head);
void ListTailAdd(struct TagListObject *node, struct TagListObject *head);
void ListDelete(struct TagListObject *node);

#define OS_LIST_FIRST(head) ((head)->next)

#define GET_TCB_PEND(nodePtr) ((struct TagTskCb *)((char *)(nodePtr) - offsetof(struct TagTskCb, pendList)))

void RegisterTcb(struct TagTskCb *tcb);
struct TagTskCb *GET_TCB_HANDLE(TskHandle id);

#define TSK_STATUS_TST(tcb, flag) (((tcb)->status & (flag)) != 0)
#define TSK_STATUS_SET(tcb, flag) ((tcb)->status |= (flag))

#define THIS_RUNQ() (&g_rq)
#define THIS_CORE() (0u)

#define OS_TSK_NULL_ID ((TskHandle)0)

#define OS_SEC_ALW_INLINE
#define OS_SEC_L0_TEXT
#define OS_SEC_L2_TEXT
#define INLINE static inline

#ifndef LIKELY
#define LIKELY(x) (x)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) (x)
#endif

#define OS_TSK_LOCK() (g_rq.uniTaskLock++)

#define OS_TASK_GET_PRI_LIST(prio) (&g_ready_queues[(prio)])

static inline void OsSpinLockTaskRq(struct TagTskCb *t)
{
    (void)t;
    g_spin_lock_called++;
}
static inline void OsSpinUnlockTaskRq(struct TagTskCb *t)
{
    (void)t;
    g_spin_unlock_called++;
}
static inline void OsRescheduleCurr(struct TagOsRunQue *rq) { rq->needReschedule = 1; }
static inline void OsTskScheduleFast(void)
{
    g_schedule_fast_called++;
    if (g_rq.needReschedule)
        g_rq.needReschedule = 0;
}
static inline void OsTskScheduleFastPs(uintptr_t intSave)
{
    (void)intSave;
    g_schedule_fast_ps_called++;
}
static inline void SMP_MC_SCHEDULE_TRIGGER(U32 core)
{
    (void)core;
    g_smp_schedule_trigger_called++;
}

static inline U32 OsIntLock(void)
{
    g_int_lock_called++;
    g_last_intsave_returned = g_next_intsave_value++;
    return g_last_intsave_returned;
}
static inline void OsIntRestore(uintptr_t intSave)
{
    g_int_restore_called++;
    g_last_intrestore_arg = (U32)intSave;
}

static inline int OS_DI_STATE_CHECK(uintptr_t intSave)
{
    (void)intSave;
    return g_di_state_check_return;
}

static inline void OsTskReadyDel(struct TagTskCb *t)
{
    (void)t;
    g_ready_del_called++;
    if (TSK_STATUS_TST(t, OS_TSK_READY))
    {
        t->status &= ~OS_TSK_READY;
    }
}
static inline void OsTskTimerAdd(struct TagTskCb *t, U32 tick)
{
    (void)t;
    (void)tick;
    g_timer_add_called++;
}

extern U32 g_last_error_code;
static inline void OS_REPORT_ERROR(U32 code) { g_last_error_code = code; }

#undef OsTaskIrqUnlock
static inline void OsTaskIrqUnlock(uintptr_t intSave)
{
    if (g_rq.uniTaskLock == 0) {
        OS_REPORT_ERROR(OS_ERRNO_TSK_UNLOCK_NO_LOCK);
        OsIntRestore(intSave);
        return;
    }

    if (--g_rq.uniTaskLock == 0) {
        if (g_rq.needReschedule != 0) {
            if ((g_rq.uniFlag & OS_HWI_ACTIVE_MASK) == 0) {
                if (OS_DI_STATE_CHECK(intSave) != 0) {
                    SMP_MC_SCHEDULE_TRIGGER(THIS_CORE());
                } else {
                    OsTskScheduleFast();
                }
            }
            g_rq.needReschedule = 0;
        }
    }
    OsIntRestore(intSave);
}

#define RUNNING_TASK (g_running_task)

void test_reset_state(void);
void test_reset_env_only(void);

extern int g_total_cases;
extern int g_pass_cases;
void test_begin(const char *title);
void test_expect_u32(const char *caseName, U32 expect, U32 actual);
void test_end(void);

#ifndef CHECK_TSK_PID_OVERFLOW
#define CHECK_TSK_PID_OVERFLOW(id) ((id) >= 64u)
#endif

#endif // COMMON_TEST_H
