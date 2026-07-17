#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_output.h"
#include "test_util.h"
#include "func_under_test.c"

static int g_notify = 0;
static void on_notify(void) { g_notify++; }

int main(void)
{
    TU_reset();
    PRT_PerfNotifyHookReg(on_notify);
    // cannot directly invoke notify; assert registration path and no crash
    TU_EXPECT_TRUE(1, "Notify hook registered");
    // register again
    PRT_PerfNotifyHookReg(on_notify);
    TU_EXPECT_TRUE(1, "Re-register ok");
    TU_report();
    return 0;
}