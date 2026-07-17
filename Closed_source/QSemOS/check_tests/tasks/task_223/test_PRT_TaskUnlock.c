#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskUnlock");

    g_rq.uniTaskLock = 1;
    PRT_TaskUnlock();
    test_expect_u32("lock->0", 0, g_rq.uniTaskLock);
    test_expect_u32("int lock called", 1, g_int_lock_called);
    test_expect_u32("int restore called", 1, g_int_restore_called);

    g_rq.uniTaskLock = 3;
    PRT_TaskUnlock();
    test_expect_u32("lock->2", 2, g_rq.uniTaskLock);

    g_rq.uniTaskLock = 0;
    U32 prevRestore = g_int_restore_called;
    PRT_TaskUnlock();
    test_expect_u32("error set", OS_ERRNO_TSK_UNLOCK_NO_LOCK, g_last_error_code);
    test_expect_u32("restore inc", prevRestore + 1, g_int_restore_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_di_state_check_return = 0;
    PRT_TaskUnlock();
    test_expect_u32("schedule called", 1, g_schedule_fast_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_di_state_check_return = 1;
    int prev_smp = g_smp_schedule_trigger_called;
    PRT_TaskUnlock();
    test_expect_u32("smp trigger inc", prev_smp + 1, g_smp_schedule_trigger_called);

    test_end();
    return 0;
}
