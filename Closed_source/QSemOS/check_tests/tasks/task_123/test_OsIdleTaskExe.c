#include "test_stubs.h"
#include "shim_prt_task_test.h"
#include "func_under_test.c"

static int sleep_calls = 0;
static void dummy_sleep(void) { sleep_calls++; }

static void reset_env(void)
{
    test_reset_stats();
    shim_reset_instrumentation();
    g_taskCoreSleep = NULL;
    sleep_calls = 0;
}

int main(void)
{
    reset_env();

    // Case 1: coreSleep is NULL -> no call
    g_taskCoreSleep = NULL;
    OsIdleTaskExe();
    EXPECT_INT_EQ("NULL coreSleep no call", 0, sleep_calls);

    // Case 2: set dummy and call once
    g_taskCoreSleep = dummy_sleep;
    OsIdleTaskExe();
    EXPECT_INT_EQ("dummy called once", 1, sleep_calls);

    // Case 3: call again
    OsIdleTaskExe();
    EXPECT_INT_EQ("dummy called twice", 2, sleep_calls);

    // Case 4: change to NULL again
    g_taskCoreSleep = NULL;
    OsIdleTaskExe();
    EXPECT_INT_EQ("no increment after NULL", 2, sleep_calls);

    // Case 5: change to dummy again
    g_taskCoreSleep = dummy_sleep;
    OsIdleTaskExe();
    EXPECT_INT_EQ("called after reassign", 3, sleep_calls);

    test_print_summary();
    return 0;
}
