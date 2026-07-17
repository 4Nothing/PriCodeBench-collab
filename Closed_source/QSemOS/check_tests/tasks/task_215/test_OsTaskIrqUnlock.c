#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

int main(void)
{
    test_reset_state();
    test_begin("OsTaskIrqUnlock");

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 0;
    g_rq.uniFlag = 0;
    g_di_state_check_return = 0;
    OsTaskIrqUnlock(123);
    test_expect_u32("lock->0", 0, g_rq.uniTaskLock);
    test_expect_u32("restore called", 1, g_int_restore_called);
    test_expect_u32("no schedule", 0, g_schedule_fast_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_rq.uniFlag = 0;
    g_di_state_check_return = 0;
    OsTaskIrqUnlock(200);
    test_expect_u32("schedule fast called", 1, g_schedule_fast_called);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_rq.uniFlag = 0;
    g_di_state_check_return = 1;
    OsTaskIrqUnlock(300);
    test_expect_u32("SMP trigger", 1, g_smp_schedule_trigger_called);

    g_rq.uniTaskLock = 3;
    g_di_state_check_return = 0;
    OsTaskIrqUnlock(400);
    test_expect_u32("lock->2", 2, g_rq.uniTaskLock);

    g_rq.uniTaskLock = 0;
    OsTaskIrqUnlock(500);
    test_expect_u32("error code set", OS_ERRNO_TSK_UNLOCK_NO_LOCK, g_last_error_code);

    g_rq.uniTaskLock = 1;
    g_rq.needReschedule = 1;
    g_rq.uniFlag = OS_HWI_ACTIVE_MASK;
    g_di_state_check_return = 0;
    OsTaskIrqUnlock(600);
    test_expect_u32("no extra schedule", 1, g_schedule_fast_called);

    test_end();
    return 0;
}
