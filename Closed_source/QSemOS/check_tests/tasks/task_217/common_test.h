// Common test harness for prt_task_lock.c unit tests
// Use include guard to prevent multiple inclusion
#ifndef COMMON_TEST_H
#define COMMON_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Define kernel types needed by auto_stub.h function pointers BEFORE including it
struct TagOsRunQue;
struct TagTskCb;

// Define OS_HWI_ACTIVE_MASK before auto_stub.h uses it
#ifndef OS_HWI_ACTIVE_MASK
#define OS_HWI_ACTIVE_MASK 0x1u
#endif

#define OS_TSK_READY (1u << 0)
#define OS_TSK_RUNNING (1u << 1)
#define OS_TSK_DELAY (1u << 2)
#define OS_TSK_PRIORITY_LOWEST 31u

// Define OS_OPTION_TASK_YIELD to enable OsTaskYield function
#define OS_OPTION_TASK_YIELD

// Include auto_stub.h - it provides all the kernel types and stubs
// auto_stub.h uses AUTO_STUB_PRT_TASK_LOCK_H as its include guard
#ifndef AUTO_STUB_PRT_TASK_LOCK_H
#include "auto_stub.h"
#endif

// NOTE: struct TagOsRunQue and struct TagTskCb are now defined in auto_stub.h

// Undefine GET_TCB_HANDLE macro so we can declare it as a function
// (auto_stub.h defines it as a macro, but tests need it as a function)
#undef GET_TCB_HANDLE

// Define test-specific globals (not in auto_stub.h)
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

// Declare functions (GET_TCB_HANDLE will be defined in common_test.c)
void RegisterTcb(struct TagTskCb *tcb);
struct TagTskCb *GET_TCB_HANDLE(TskHandle id);

#define TSK_STATUS_TST(tcb, flag) (((tcb)->status & (flag)) != 0)
#define TSK_STATUS_SET(tcb, flag) ((tcb)->status |= (flag))

// Test harness for g_rq - g_runningTask is defined in auto_stub.h
extern struct TagOsRunQue g_rq;
#define THIS_RUNQ() (&g_rq)
#define THIS_CORE() (0u)

// Use g_runningTask (with 'T') which is what auto_stub.h defines
#define RUNNING_TASK ((struct TagTskCb *)g_runningTask)

// g_running_task (lowercase t) is used by some tests
extern struct TagTskCb *g_running_task;

#define OS_TSK_NULL_ID ((TskHandle)0)

#define OS_SEC_ALW_INLINE
#define OS_SEC_L0_TEXT
#define OS_SEC_L2_TEXT

#ifndef LIKELY
#define LIKELY(x) (x)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) (x)
#endif

#define OS_TSK_LOCK() (g_rq.uniTaskLock++)

#define OS_TASK_GET_PRI_LIST(prio) (&g_ready_queues[(prio)])

// OS_INT_ACTIVE and OS_TASK_LOCK_DATA are macros in auto_stub.h
// We need to undef them to declare as variables for test harness
#undef OS_INT_ACTIVE
#undef OS_TASK_LOCK_DATA
extern volatile U32 OS_INT_ACTIVE;
extern volatile U32 OS_TASK_LOCK_DATA;

// Declare missing functions that prt_task_lock.c needs
static inline uintptr_t OsIntLock(void) {
    g_int_lock_called++;
    g_last_intsave_returned = g_next_intsave_value++;
    return g_last_intsave_returned;
}

static inline void OsIntRestore(uintptr_t intSave) {
    g_int_restore_called++;
    g_last_intrestore_arg = (U32)intSave;
}

static inline int OS_DI_STATE_CHECK(uintptr_t intSave) {
    (void)intSave;
    return g_di_state_check_return;
}

static inline void SMP_MC_SCHEDULE_TRIGGER(U32 core) {
    (void)core;
    g_smp_schedule_trigger_called++;
}

static inline void OsRescheduleCurr(struct TagOsRunQue *rq) {
    (void)rq;
}

static inline void OsTskScheduleFast(void) {
    g_schedule_fast_called++;
    if (g_rq.needReschedule)
        g_rq.needReschedule = 0;
}

extern U32 g_last_error_code;
static inline void OS_REPORT_ERROR(U32 code) { g_last_error_code = code; }

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
