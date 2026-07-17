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

static void offA(void) {}
static void offB(void) {}

int main(void)
{
    reset_env();

    // Case 1: set A
    OsSetOfflineFlagHook(offA);
    EXPECT_PTR_EQ("assign offA", offA, g_setOfflineFlagHook);

    // Case 2: set B
    OsSetOfflineFlagHook(offB);
    EXPECT_PTR_EQ("overwrite to offB", offB, g_setOfflineFlagHook);

    // Case 3: set NULL
    OsSetOfflineFlagHook(NULL);
    EXPECT_PTR_EQ("assign NULL", NULL, g_setOfflineFlagHook);

    // Case 4: assign same again
    OsSetOfflineFlagHook(NULL);
    EXPECT_PTR_EQ("assign NULL again", NULL, g_setOfflineFlagHook);

    // Case 5: back to A
    OsSetOfflineFlagHook(offA);
    EXPECT_PTR_EQ("back to offA", offA, g_setOfflineFlagHook);

    test_print_summary();
    return 0;
}
