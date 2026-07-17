#include <stdio.h>
#include <string.h>

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

    /* Case 1: prev == next -> no hook */
    init_task(&g_taskA, 11, 0, 0);
    OsContextSwitch(&g_taskA, &g_taskA);
    assert_case("No hook when same", 0, g_switchHookCount);

    /* Case 2: prev != next -> hook called once */
    reset_test_state();
    init_task(&g_taskA, 11, 0, 0);
    init_task(&g_taskB, 22, 0, 0);
    OsContextSwitch(&g_taskA, &g_taskB);
    assert_case("Hook called once", 1, g_switchHookCount);

    /* Case 3: context load target == next */
    assert_case("Context load next", g_taskB.taskPid, ((struct TagTskCb *)g_lastContextLoadPtr)->taskPid);

    /* Case 4: check last PIDs recorded */
    assert_case("Hook prev pid recorded", 11, g_lastPrevPid);
    assert_case("Hook next pid recorded", 22, g_lastNextPid);

    /* Case 5: multiple switches accumulate count */
    OsContextSwitch(&g_taskB, &g_taskA);
    OsContextSwitch(&g_taskA, &g_taskB);
    assert_case("Multiple switches count", 3, g_switchHookCount);

    printf("Pass-Rate: %.2f%%\n", total ? (100.0 * pass / total) : 0.0);
    return (pass == total) ? 0 : 1;
}
