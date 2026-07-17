#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "test_util.h"
#include "func_under_test.c"

static int g_start_calls = 0;
static const char *name_fn(Event *e)
{
    (void)e;
    return "evt";
}
static U32 pmu_cfg(void) { return OS_OK; }
static U32 pmu_start(void)
{
    g_start_calls++;
    return OS_OK;
}
static U32 pmu_stop(void) { return OS_OK; }
static Pmu g_pmux = {.events = {{0}}, .type = 0, .config = pmu_cfg, .start = pmu_start, .stop = pmu_stop, .getName = name_fn};
Pmu *OsPerfPmuGet(U32 type)
{
    g_pmux.type = type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();
    // prepare minimal pmu
    (void)OsPerfConfig(&(PerfEventConfig){.type = 1, .eventsNr = 0, .predivided = 0});

    // 1) pmu not registered -> no change
    g_pmu = NULL;
    g_start_calls = 0;
    OsPerfStart();
    TU_EXPECT_EQ_U32(0, g_start_calls, "No PMU registered");

    // 2) pmu registered, status not started -> start called
    g_pmu = &g_pmux;
    g_start_calls = 0;
    g_perfCb.pmuStatusPerCpu[0] = PERF_PMU_STOPED;
    OsPerfStart();
    TU_EXPECT_EQ_U32(1, g_start_calls, "Start invoked once");

    // 3) status set to started
    TU_EXPECT_EQ_U32(PERF_PMU_STARTED, g_perfCb.pmuStatusPerCpu[0], "Per-cpu status started");

    // 4) second start does not call again
    OsPerfStart();
    TU_EXPECT_EQ_U32(1, g_start_calls, "No duplicate start");

    // 5) different cpu index path (simulate by writing different index)
    g_perfCb.pmuStatusPerCpu[1] = PERF_PMU_STOPED; // simulate other core array slot
    OsPerfStart();                                 // still current core 0, so count unchanged
    TU_EXPECT_EQ_U32(1, g_start_calls, "Other cpu not affected in this test");

    TU_report();
    return 0;
}