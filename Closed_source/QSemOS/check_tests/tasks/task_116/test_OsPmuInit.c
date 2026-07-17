#define OS_OPTION_PERF_HW_PMU
#define OS_OPTION_PERF_TIMED_PMU
#define OS_OPTION_PERF_SW_PMU

#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "mock_include/prt_perf_output.h"
#include "test_util.h"
#include "func_under_test.c"

// Control flags to simulate init results
static U32 g_hw_ret = OS_OK;
static U32 g_timed_ret = OS_OK;
static U32 g_sw_ret = OS_OK;

U32 OsHwPmuInit(void) { return g_hw_ret; }
U32 OsTimedPmuInit(void) { return g_timed_ret; }
U32 OsSwPmuInit(void) { return g_sw_ret; }

// /* OsPmuInit now from source */


int main(void)
{
    TU_reset();

    g_hw_ret = OS_OK;
    g_timed_ret = OS_OK;
    g_sw_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_OK, OsPmuInit(), "All init OK");

    g_hw_ret = 0xDEAD;
    g_timed_ret = OS_OK;
    g_sw_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_HW_INIT_ERROR, OsPmuInit(), "HW init fails");

    g_hw_ret = OS_OK;
    g_timed_ret = 0xBEEF;
    g_sw_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_TIMED_INIT_ERROR, OsPmuInit(), "TIMED init fails");

    g_hw_ret = OS_OK;
    g_timed_ret = OS_OK;
    g_sw_ret = 0xFACE;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_SW_INIT_ERROR, OsPmuInit(), "SW init fails");

    g_hw_ret = OS_OK;
    g_timed_ret = OS_OK;
    g_sw_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_OK, OsPmuInit(), "Init succeeds again");

    TU_report();
    return 0;
}