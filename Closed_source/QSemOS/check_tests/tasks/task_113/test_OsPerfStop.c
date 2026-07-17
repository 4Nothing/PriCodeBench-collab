#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "test_util.h"
#include "func_under_test.c"

static int g_stop_calls = 0;
static const char *name_fn(Event *e)
{
    (void)e;
    return "evt";
}
static U32 pmu_cfg(void) { return OS_OK; }
static U32 pmu_start(void) { return OS_OK; }
static U32 pmu_stop(void)
{
    g_stop_calls++;
    return OS_OK;
}
static Pmu g_pmux = {.events = {{0}}, .type = 0, .config = pmu_cfg, .start = pmu_start, .stop = pmu_stop, .getName = name_fn};
Pmu *OsPerfPmuGet(U32 type)
{
    g_pmux.type = type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();

    // 1) pmu not registered
    g_pmu = NULL;
    g_stop_calls = 0;
    OsPerfStop();
    TU_EXPECT_EQ_U32(0, g_stop_calls, "No PMU registered");

    // 2) pmu registered, status not stoped -> stop called
    g_pmu = &g_pmux;
    g_stop_calls = 0;
    g_perfCb.pmuStatusPerCpu[0] = PERF_PMU_STARTED;
    g_perfCb.needStoreToBuffer = false;
    // set one event with period to go through printing path
    g_pmu->events.nr = 1;
    g_pmu->events.per[0].period = 10;
    g_pmu->events.per[0].eventId = 1;
    OsPerfStop();
    TU_EXPECT_EQ_U32(1, g_stop_calls, "Stop invoked once");

    // 3) per-cpu status updated
    TU_EXPECT_EQ_U32(PERF_PMU_STOPED, g_perfCb.pmuStatusPerCpu[0], "Per-cpu status stoped");

    // 4) repeated stop does not call again
    OsPerfStop();
    TU_EXPECT_EQ_U32(1, g_stop_calls, "No duplicate stop");

    // 5) with needStoreToBuffer true, count printing skipped (can't assert print; ensure no extra stop)
    g_perfCb.pmuStatusPerCpu[0] = PERF_PMU_STARTED;
    g_perfCb.needStoreToBuffer = true;
    OsPerfStop();
    TU_EXPECT_EQ_U32(2, g_stop_calls, "Stop called with buffer mode");

    TU_report();
    return 0;
}