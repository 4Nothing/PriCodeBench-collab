#include "common/test_common.h"
#include "common/shim_prt_task_test.h"
#include "func_under_test.c"


static void reset_env(void)
{
    test_reset_stats();
    shim_reset_instrumentation();
}

static struct TagTskCb make_task(int core, int pid, bool onRq)
{
    struct TagTskCb t = {0};
    t.coreID = core;
    t.taskPid = pid;
    t.isOnRq = onRq;
    return t;
}

int main(void)
{
    reset_env();

    // Case 1: add and ensure no-wake resched variant used
    struct TagTskCb t1 = make_task(0, 21, false);
    OsTskReadyAddNoWakeUpIpc(&t1);
    EXPECT_INT_EQ("enqueue called", 1, g_calls_OsEnqueueTask);
    EXPECT_INT_EQ("reschedNoWake called", 1, g_calls_OsReschedTaskNoWakeIpc);

    // Case 2: adding existing on rq does not re-enqueue
    OsTskReadyAddNoWakeUpIpc(&t1);
    EXPECT_INT_EQ("enqueue still 1", 1, g_calls_OsEnqueueTask);

    // Case 3: another core
    struct TagTskCb t2 = make_task(1, 22, false);
    OsTskReadyAddNoWakeUpIpc(&t2);
    EXPECT_INT_EQ("nrRunning core1 one", 1, GET_RUNQ(1)->nrRunning);

    // Case 4: check ready flag set
    EXPECT_BOOL_EQ("t1 ready flag", true, (t1.status & OS_TSK_READY) != 0);
    EXPECT_BOOL_EQ("t2 ready flag", true, (t2.status & OS_TSK_READY) != 0);

    // Case 5: timeSlice set from global
    EXPECT_UINT_EQ("t1 timeSlice", g_timeSliceCycle, t1.timeSlice);

    test_print_summary();
    return 0;
}
