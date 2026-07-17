#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskIrqUnlock");

    g_rq.uniTaskLock = 2;
    U32 prevRestore = g_int_restore_called;
    PRT_TaskIrqUnlock(777);
    test_expect_u32("lock->1", 1, g_rq.uniTaskLock);
    test_expect_u32("restore inc", prevRestore + 1, g_int_restore_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_di_state_check_return = 0;
    int prev_sched = g_schedule_fast_called;
    PRT_TaskIrqUnlock(1000);
    test_expect_u32("schedule inc", prev_sched + 1, g_schedule_fast_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_rq.uniFlag = OS_HWI_ACTIVE_MASK;
    g_di_state_check_return = 0;
    prev_sched = g_schedule_fast_called;
    PRT_TaskIrqUnlock(2000);
    test_expect_u32("no schedule inc", prev_sched, g_schedule_fast_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_rq.uniFlag = 0;
    g_di_state_check_return = 1;
    int prev_smp = g_smp_schedule_trigger_called;
    PRT_TaskIrqUnlock(3000);
    test_expect_u32("smp trigger inc", prev_smp + 1, g_smp_schedule_trigger_called);

    g_rq.uniTaskLock = 0;
    PRT_TaskIrqUnlock(4000);
    test_expect_u32("error set", OS_ERRNO_TSK_UNLOCK_NO_LOCK, g_last_error_code);

    test_end();
    return 0;
}
