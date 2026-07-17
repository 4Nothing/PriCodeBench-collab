#define OS_OPTION_SMP
#define OS_OPTION_PERF_HW_PMU
#define OS_OPTION_PERF_TIMED_PMU
#define OS_OPTION_PERF_SW_PMU

#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_output.h"
#include "test_util.h"
#include "func_under_test.c"

static U32 g_hw_ret = OS_OK, g_timed_ret = OS_OK, g_sw_ret = OS_OK;
U32 OsHwPmuInit(void) { return g_hw_ret; }
U32 OsTimedPmuInit(void) { return g_timed_ret; }
U32 OsSwPmuInit(void) { return g_sw_ret; }

static U32 g_out_init_ret = OS_OK;
U32 OsPerfOutPutInit(void *b, U32 s)
{
    (void)b;
    (void)s;
    return g_out_init_ret;
}

// Stub for OsPmuInit
U32 OsPmuInit(void)
{
    if (g_hw_ret != OS_OK) return OS_ERRNO_PERF_HW_INIT_ERROR;
    if (g_timed_ret != OS_OK) return OS_ERRNO_PERF_TIMED_INIT_ERROR;
    if (g_sw_ret != OS_OK) return OS_ERRNO_PERF_SW_INIT_ERROR;
    return OS_OK;
}

static char buf[128];

int main(void)
{
    TU_reset();
    g_perfCb.status = PERF_UNINIT;

    // 1) all OK
    g_hw_ret = g_timed_ret = g_sw_ret = OS_OK;
    g_out_init_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_OK, PRT_PerfInit(buf, sizeof(buf)), "Init OK");

    // 2) repeated init -> status invalid
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_STATUS_INVALID, PRT_PerfInit(buf, sizeof(buf)), "Init when already in STOPED");

    // reset status manual for negative tests
    g_perfCb.status = PERF_UNINIT;
    g_hw_ret = 0x1;
    g_timed_ret = OS_OK;
    g_sw_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_HW_INIT_ERROR, PRT_PerfInit(buf, sizeof(buf)), "HW init fails");

    g_perfCb.status = PERF_UNINIT;
    g_hw_ret = OS_OK;
    g_timed_ret = 0x2;
    g_sw_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_TIMED_INIT_ERROR, PRT_PerfInit(buf, sizeof(buf)), "TIMED init fails");

    g_perfCb.status = PERF_UNINIT;
    g_hw_ret = OS_OK;
    g_timed_ret = OS_OK;
    g_sw_ret = 0x3;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_SW_INIT_ERROR, PRT_PerfInit(buf, sizeof(buf)), "SW init fails");

    // 5) buffer init fails
    g_perfCb.status = PERF_UNINIT;
    g_hw_ret = g_timed_ret = g_sw_ret = OS_OK;
    g_out_init_ret = OS_ERROR;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_BUF_ERROR, PRT_PerfInit(buf, sizeof(buf)), "Buffer init fails");

    TU_report();
    return 0;
}