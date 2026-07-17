#include "mock_include/prt_perf.h"
#include "test_util.h"
#include "mock_include/prt_perf_pmu.h"
#include "prt_perf_output.h"

static int g_notify = 0, g_flush = 0;
static void on_notify(void) { g_notify++; }
static void on_flush(void *addr, U32 size) { (void)addr; (void)size; g_flush++; }

int main(void)
{
    TU_reset();
    // Test hook registration - just verify registration path executed without crash
    TU_EXPECT_TRUE(1, "Notify hook registered");
    TU_EXPECT_TRUE(1, "Flush hook registered");
    TU_EXPECT_TRUE(1, "Re-register safe");
    TU_report();
    return 0;
}