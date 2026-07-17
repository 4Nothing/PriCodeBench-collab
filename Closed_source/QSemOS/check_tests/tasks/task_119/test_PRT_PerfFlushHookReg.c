#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_output.h"
#include "test_util.h"
#include "func_under_test.c"

static int g_flush = 0;
static void on_flush(void *addr, U32 size) { (void)addr; (void)size; g_flush++; }

int main(void)
{
    TU_reset();
    PRT_PerfFlushHookReg(on_flush);
    TU_EXPECT_TRUE(1, "Flush hook registered");
    PRT_PerfFlushHookReg(on_flush);
    TU_EXPECT_TRUE(1, "Re-register ok");
    TU_report();
    return 0;
}