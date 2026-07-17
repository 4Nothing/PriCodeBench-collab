#include <stdio.h>
#include <string.h>

#define OS_OPTION_RR_SCHED 1
#define OS_OPTION_RR_SCHED_IRQ_TIME_DISCOUNT 1
#define OS_SEC_L0_TEXT

#include "test_support.h"
#include "prt_hook_external.h"
#include "prt_task_external.h"
#include "prt_irq_external.h"
#include "func_under_test.c"

/* Include the implementation under test */

static int pass = 0, total = 0;
static void assert_case(const char *name, int expected, int actual)
{
    total++;
    printf("[Case] %s\n  Expect: %d\n  Actual: %d\n", name, expected, actual);
    if (expected == actual)
    {
        pass++;
        printf("  Result: PASS\n");
    }
    else
    {
        printf("  Result: FAIL\n");
    }
}

int main(void)
{
    reset_test_state();

    /* Case 1: currTsk NULL -> no change */
    RUNNING_TASK = NULL;
    OsHwiEndCheckTimeSlice(0);
    assert_case("NULL current task", 0, g_contextLoadCount);

    /* Prepare a RR task running */
    reset_test_state();
    RUNNING_TASK = &g_taskB; // RR with timeSlice 3
    g_highestTask = RUNNING_TASK;
    g_uniTaskLock = 0;

    /* Case 2: policy not RR -> no scheduling request */
    RUNNING_TASK->policy = 0; // not RR
    UNI_FLAG = 0;
    OsHwiEndCheckTimeSlice(0);
    assert_case("Non-RR policy no req", 0, (UNI_FLAG & OS_FLG_TSK_REQ) != 0);

    /* Case 3: RR but timeSlice remains > 0 -> no req */
    RUNNING_TASK->policy = OS_TSK_SCHED_RR;
    RUNNING_TASK->timeSlice = 2;
    UNI_FLAG = 0;
    OsHwiEndCheckTimeSlice(0);
    assert_case("RR timeslice > 0 no req", 0, (UNI_FLAG & OS_FLG_TSK_REQ) != 0);

    /* Case 4: RR timeSlice reaches 0 -> set req if higher task exists */
    RUNNING_TASK->timeSlice = 1;
    RUNNING_TASK->irqUsedTime = 0;
    /* Set startTime to a value less than currTime so time slice gets updated */
    RUNNING_TASK->startTime = 0;
    /* Stub update will decrement to 0 and requeue; make highest different */
    struct TagTskCb other;
    init_task(&other, 9, OS_TSK_SCHED_RR, 5);
    g_highestTask = &other;
    UNI_FLAG = 0;
    OsHwiEndCheckTimeSlice(1);  /* Use currTime > startTime */
    assert_case("RR timeslice -> 0 req set", 1, (UNI_FLAG & OS_FLG_TSK_REQ) ? 1 : 0);

    /* Case 5: RR timeSlice 0 but highest == current -> no req */
    reset_test_state();
    RUNNING_TASK = &g_taskB;
    g_highestTask = RUNNING_TASK;
    RUNNING_TASK->policy = OS_TSK_SCHED_RR;
    RUNNING_TASK->timeSlice = 1;
    UNI_FLAG = 0;
    OsHwiEndCheckTimeSlice(0);
    assert_case("RR timeslice -> 0 highest==curr", 0, (UNI_FLAG & OS_FLG_TSK_REQ) != 0);

    /* Case 6: uniTaskLock != 0 -> no req even if highest changed */
    reset_test_state();
    RUNNING_TASK = &g_taskB;
    struct TagTskCb high2;
    init_task(&high2, 7, OS_TSK_SCHED_RR, 5);
    g_highestTask = &high2;
    RUNNING_TASK->policy = OS_TSK_SCHED_RR;
    RUNNING_TASK->timeSlice = 1;
    g_uniTaskLock = 1;
    UNI_FLAG = 0;
    OsHwiEndCheckTimeSlice(0);
    assert_case("RR timeslice -> 0 but locked", 0, (UNI_FLAG & OS_FLG_TSK_REQ) != 0);

    printf("Pass-Rate: %.2f%%\n", total ? (100.0 * pass / total) : 0.0);
    return (pass == total) ? 0 : 1;
}
