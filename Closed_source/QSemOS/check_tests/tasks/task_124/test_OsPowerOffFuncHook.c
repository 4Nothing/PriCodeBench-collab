#include "common/test_common.h"
#include "common/shim_prt_task_test.h"
#include "func_under_test.c"


static void reset_env(void)
{
    test_reset_stats();
    shim_reset_instrumentation();
    g_sysPowerOffHook = NULL;
    g_setOfflineFlagHook = NULL;
}

static void hookA(void) {}
static void hookB(void) {}

int main(void)
{
    reset_env();

    // Case 1: set non-NULL
    OsPowerOffFuncHook(hookA);
    EXPECT_PTR_EQ("assign hookA", hookA, g_sysPowerOffHook);

    // Case 2: overwrite with another
    OsPowerOffFuncHook(hookB);
    EXPECT_PTR_EQ("overwrite to hookB", hookB, g_sysPowerOffHook);

    // Case 3: set NULL
    OsPowerOffFuncHook(NULL);
    EXPECT_PTR_EQ("assign NULL", NULL, g_sysPowerOffHook);

    // Case 4: set same pointer again
    OsPowerOffFuncHook(NULL);
    EXPECT_PTR_EQ("assign NULL again", NULL, g_sysPowerOffHook);

    // Case 5: back to hookA
    OsPowerOffFuncHook(hookA);
    EXPECT_PTR_EQ("back to hookA", hookA, g_sysPowerOffHook);

    test_print_summary();
    return 0;
}
