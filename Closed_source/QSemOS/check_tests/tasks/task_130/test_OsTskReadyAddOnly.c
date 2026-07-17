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
    t.status = 0;
    t.timeSlice = 0;
    return t;
}

int main(void)
{
    reset_env();

    // Case 1: normal enqueue
    struct TagTskCb t1 = make_task(0, 1, false);
    OsTskReadyAddOnly(&t1);
    EXPECT_BOOL_EQ("ready flag set", true, (t1.status & OS_TSK_READY) != 0);
    EXPECT_BOOL_EQ("isOnRq true", true, t1.isOnRq);
    EXPECT_UINT_EQ("timeSlice set", g_timeSliceCycle, t1.timeSlice);
    EXPECT_INT_EQ("nrRunning inc core0", 1, GET_RUNQ(0)->nrRunning);

    // Case 2: add again when already on rq -> no double changes
    OsTskReadyAddOnly(&t1);
    EXPECT_INT_EQ("enqueue once", 1, g_calls_OsEnqueueTask);
    EXPECT_INT_EQ("inc once", 1, g_calls_OsIncNrRunning);

    // Case 3: another task on different core
    struct TagTskCb t2 = make_task(1, 2, false);
    OsTskReadyAddOnly(&t2);
    EXPECT_INT_EQ("nrRunning core1", 1, GET_RUNQ(1)->nrRunning);

    // Case 4: time slice reflects global cycle
    g_timeSliceCycle = 7;
    struct TagTskCb t3 = make_task(0, 3, false);
    OsTskReadyAddOnly(&t3);
    EXPECT_UINT_EQ("timeSlice uses new cycle", 7, t3.timeSlice);

    // Case 5: large core index wraps by macro
    struct TagTskCb t4 = make_task(10, 4, false);
    OsTskReadyAddOnly(&t4);
    EXPECT_INT_EQ("nrRunning wrapped core", 3, GET_RUNQ(0)->nrRunning);

    test_print_summary();
    return 0;
}
