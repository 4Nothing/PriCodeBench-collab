#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskLock");

    PRT_TaskLock();
    test_expect_u32("uniTaskLock==1", 1, g_rq.uniTaskLock);
    test_expect_u32("OsIntLock called", 1, g_int_lock_called);
    test_expect_u32("OsIntRestore called", 1, g_int_restore_called);

    PRT_TaskLock();
    test_expect_u32("uniTaskLock==2", 2, g_rq.uniTaskLock);
    test_expect_u32("restore arg matches", g_last_intsave_returned, g_last_intrestore_arg);
    test_expect_u32("schedule_fast not called", 0, g_schedule_fast_called);

    OS_INT_ACTIVE = 1;
    PRT_TaskLock();
    test_expect_u32("uniTaskLock==3", 3, g_rq.uniTaskLock);

    test_end();
    return 0;
}
