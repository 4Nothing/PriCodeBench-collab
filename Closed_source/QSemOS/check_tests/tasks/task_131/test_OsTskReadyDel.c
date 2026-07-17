// shim_prt_task_test.h now includes auto_stub.h internally
// auto_stub.h's OsTskReadyDel stub calls OsTskReadyDelImpl which we provide in shim
#include "common/shim_prt_task_test.h"
#include "common/test_common.h"
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
    t.taskStatus = OS_TSK_READY;
    return t;
}

int main(void)
{
    reset_env();

    // Prepare runqueue state
    struct TagTskCb t1 = make_task(0, 31, true);
    GET_RUNQ(0)->nrRunning = 1;

    // Case 1: delete from rq
    OsTskReadyDel(&t1);
    EXPECT_BOOL_EQ("ready flag cleared", false, (t1.taskStatus & OS_TSK_READY) != 0);
    EXPECT_BOOL_EQ("isOnRq false", false, t1.isOnRq);
    EXPECT_INT_EQ("nrRunning dec", 0, GET_RUNQ(0)->nrRunning);

    // Case 2: deleting not on rq -> no change
    struct TagTskCb t2 = make_task(0, 32, false);
    t2.taskStatus |= OS_TSK_READY;
    OsTskReadyDel(&t2);
    EXPECT_INT_EQ("dequeue called only once", 1, g_calls_OsDequeueTask);

    // Case 3: when task is current, resched called
    struct TagTskCb t3 = make_task(0, 33, true);
    t3.taskStatus |= OS_TSK_READY;
    GET_RUNQ(0)->nrRunning = 1;
    GET_RUNQ(0)->tskCurr = &t3;
    OsTskReadyDel(&t3);
    EXPECT_INT_EQ("resched called", 1, g_calls_OsReschedTask);

    // Case 4: not current -> resched not incremented further
    struct TagTskCb t4 = make_task(0, 34, true);
    t4.taskStatus |= OS_TSK_READY;
    GET_RUNQ(0)->nrRunning = 1;
    GET_RUNQ(0)->tskCurr = NULL;
    OsTskReadyDel(&t4);
    EXPECT_INT_EQ("resched still 1", 1, g_calls_OsReschedTask);

    // Case 5: different core
    struct TagTskCb t5 = make_task(1, 35, true);
    t5.taskStatus |= OS_TSK_READY;
    GET_RUNQ(1)->nrRunning = 1;
    OsTskReadyDel(&t5);
    EXPECT_INT_EQ("nrRunning core1 dec", 0, GET_RUNQ(1)->nrRunning);

    test_print_summary();
    return 0;
}
