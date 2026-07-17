#include <stdio.h>
#include <string.h>

#define OS_OPTION_RR_SCHED 1
#define OS_SEC_L0_TEXT

#include "test_support.h"
#include "prt_hook_external.h"
#include "prt_task_external.h"
#include "prt_irq_external.h"
#include "func_under_test.c"


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

    /* Case 1: no request flag -> just context load current */
    RUNNING_TASK = &g_taskA;
    g_highestTask = RUNNING_TASK;
    UNI_FLAG = 0;
    g_contextLoadCount = 0;
    OsMainSchedule();
    assert_case("No req flag loads current", 1, g_contextLoadCount);

    /* Case 2: request flag set, prev != highest -> switch and load highest */
    reset_test_state();
    RUNNING_TASK = &g_taskA;
    init_task(&g_taskA, 1, 0, 0);
    init_task(&g_taskB, 2, OS_TSK_SCHED_RR, 3);
    g_highestTask = &g_taskB;
    UNI_FLAG = OS_FLG_TSK_REQ;
    OsMainSchedule();
    assert_case("Switch to highest", g_taskB.taskPid, ((struct TagTskCb *)g_lastContextLoadPtr)->taskPid);

    /* Case 3: switch hook called when prev != highest */
    assert_case("Switch hook called", 1, g_switchHookCount > 0);

    /* Case 4: request flag cleared after schedule */
    assert_case("Req flag cleared", 0, (UNI_FLAG & OS_FLG_TSK_REQ) != 0);

    /* Case 5: prev == highest -> no extra hook call */
    int before = g_switchHookCount;
    UNI_FLAG = OS_FLG_TSK_REQ;
    OsMainSchedule();
    assert_case("Prev==Highest no extra hook", before, g_switchHookCount);

    /* Case 6: RR update called when option enabled */
    assert_case("RR update startTime set", 1, g_taskB.startTime == g_fakeTime ? 1 : 0);

    printf("Pass-Rate: %.2f%%\n", total ? (100.0 * pass / total) : 0.0);
    return (pass == total) ? 0 : 1;
}
