#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "test_util.h"
#include "func_under_test.c"

static const char *name_fn(Event *e)
{
    (void)e;
    return "evt";
}
static U32 pmu_cfg(void) { return OS_OK; }
static U32 pmu_start(void) { return OS_OK; }
static U32 pmu_stop(void) { return OS_OK; }
static Pmu g_pmux = {.events = {0}, .type = 0, .config = pmu_cfg, .start = pmu_start, .stop = pmu_stop, .getName = name_fn};
Pmu *OsPerfPmuGet(U32 type)
{
    g_pmux.type = type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();

    // 1) pmu null -> false
    g_pmu = NULL;
    TU_EXPECT_TRUE(!OsPerfParamValid(), "PMU null invalid");

    // 2) events nr 0 -> false
    g_pmu = &g_pmux;
    g_pmu->events.nr = 0;
    TU_EXPECT_TRUE(!OsPerfParamValid(), "No events invalid");

    // 3) all periods zero -> false
    g_pmu->events.nr = 2;
    g_pmu->events.per[0].period = 0;
    g_pmu->events.per[1].period = 0;
    TU_EXPECT_TRUE(!OsPerfParamValid(), "Zero periods invalid");

    // 4) any non-zero -> true
    g_pmu->events.per[1].period = 10;
    TU_EXPECT_TRUE(OsPerfParamValid(), "One non-zero valid");

    // 5) multiple non-zero -> true
    g_pmu->events.per[0].period = 5;
    TU_EXPECT_TRUE(OsPerfParamValid(), "Multiple non-zero valid");

    TU_report();
    return 0;
}