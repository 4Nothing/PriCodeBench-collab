#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskIrqLock");

    U32 ret1 = PRT_TaskIrqLock();
    test_expect_u32("lock==1", 1, g_rq.uniTaskLock);
    test_expect_u32("ret==first intsave", 100, ret1);

    U32 ret2 = PRT_TaskIrqLock();
    test_expect_u32("lock==2", 2, g_rq.uniTaskLock);
    test_expect_u32("ret==next intsave", 101, ret2);

    test_expect_u32("no restore yet", 0, g_int_restore_called);
    test_expect_u32("no scheduling", 0, g_schedule_fast_called);

    g_rq.needReschedule = 1;
    U32 ret3 = PRT_TaskIrqLock();
    test_expect_u32("lock==3", 3, g_rq.uniTaskLock);
    test_expect_u32("ret progresses", 102, ret3);

    test_end();
    return 0;
}
