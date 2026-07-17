#include <stdio.h>
#include <string.h>

#define OS_OPTION_RR_SCHED 1
#define OS_SEC_L4_TEXT

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

    /* Case 1: highest set by helper to B */
    OsFirstTimeSwitch();
    assert_case("Highest set", g_taskB.taskPid, ((struct TagTskCb *)g_lastContextLoadPtr)->taskPid);

    /* Case 2: running task points to highest */
    assert_case("RUNNING_TASK == highest", 1, RUNNING_TASK == g_highestTask);

    /* Case 3: OS_TSK_RUNNING flag set */
    assert_case("Running flag set", 1, (RUNNING_TASK->taskStatus & OS_TSK_RUNNING) ? 1 : 0);

    /* Case 4: startTime initialized when RR option */
    assert_case("startTime initialized", 1, RUNNING_TASK->startTime == g_fakeTime ? 1 : 0);

    /* Case 5: context load called exactly once */
    assert_case("Context loaded once", 1, g_contextLoadCount);

    printf("Pass-Rate: %.2f%%\n", total ? (100.0 * pass / total) : 0.0);
    return (pass == total) ? 0 : 1;
}
