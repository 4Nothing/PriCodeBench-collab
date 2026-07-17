#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskLockNoIntLock");

    PRT_TaskLockNoIntLock();
    test_expect_u32("lock becomes 1", 1, g_rq.uniTaskLock);
    test_expect_u32("no OsIntLock called", 0, g_int_lock_called);
    test_expect_u32("no OsIntRestore called", 0, g_int_restore_called);

    PRT_TaskLockNoIntLock();
    PRT_TaskLockNoIntLock();
    test_expect_u32("lock becomes 3", 3, g_rq.uniTaskLock);

    test_expect_u32("needReschedule unchanged", 0, g_rq.needReschedule);
    test_expect_u32("schedule_fast not called", 0, g_schedule_fast_called);

    for (int i = 0; i < 5; ++i)
        PRT_TaskLockNoIntLock();
    test_expect_u32("lock becomes 8", 8, g_rq.uniTaskLock);

    test_end();
    return 0;
}
