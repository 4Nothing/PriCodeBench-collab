#include "common/test_common.h"
#include "common/shim_prt_task_test.h"
#include "func_under_test.c"

// Include the source under test

static void reset_env(void)
{
    test_reset_stats();
    shim_reset_instrumentation();
    g_sysPowerOffHook = NULL;
    g_setOfflineFlagHook = NULL;
}

int main(void)
{
    reset_env();

    // Case 1: default false -> set true
    g_sysPowerOffFlag = false;
    OsPowerOffSetFlag();
    EXPECT_BOOL_EQ("flag set from false", true, g_sysPowerOffFlag);

    // Case 2: already true -> remains true
    g_sysPowerOffFlag = true;
    OsPowerOffSetFlag();
    EXPECT_BOOL_EQ("flag remains true", true, g_sysPowerOffFlag);

    // Case 3: multiple calls -> still true
    g_sysPowerOffFlag = false;
    OsPowerOffSetFlag();
    OsPowerOffSetFlag();
    EXPECT_BOOL_EQ("multiple calls idempotent", true, g_sysPowerOffFlag);

    // Case 4: reset to false manually then set
    g_sysPowerOffFlag = false;
    OsPowerOffSetFlag();
    EXPECT_BOOL_EQ("manual reset then set", true, g_sysPowerOffFlag);

    // Case 5: hooks unaffected
    g_sysPowerOffHook = (PowerOffFuncT)0x1234;
    OsPowerOffSetFlag();
    EXPECT_PTR_EQ("hook not modified", (void *)0x1234, (void *)g_sysPowerOffHook);

    test_print_summary();
    return 0;
}
