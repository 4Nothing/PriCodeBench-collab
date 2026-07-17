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

    // Case 1: add new task triggers trace and resched
    struct TagTskCb t1 = make_task(0, 11, false);
    OsTskReadyAdd(&t1);
    EXPECT_INT_EQ("trace pid captured", -1, g_last_sys_trace_pid);
    EXPECT_INT_EQ("resched called", 1, g_calls_OsReschedTask);

    // Case 2: add another task increments runqueue
    struct TagTskCb t2 = make_task(0, 12, false);
    OsTskReadyAdd(&t2);
    EXPECT_INT_EQ("nrRunning core0 two", 2, GET_RUNQ(0)->nrRunning);

    // Case 3: add task already on rq does not enqueue
    struct TagTskCb t3 = make_task(0, 13, true);
    OsTskReadyAdd(&t3);
    EXPECT_INT_EQ("enqueue count still 2", 2, g_calls_OsEnqueueTask);

    // Case 4: different core
    struct TagTskCb t4 = make_task(1, 14, false);
    OsTskReadyAdd(&t4);
    EXPECT_INT_EQ("nrRunning core1 one", 1, GET_RUNQ(1)->nrRunning);

    // Case 5: pid traced per call
    EXPECT_INT_EQ("last pid is t4", -1, g_last_sys_trace_pid);

    test_print_summary();
    return 0;
}
