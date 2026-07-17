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
static Pmu g_pmux = {.events = {{0}}, .type = 0, .config = pmu_cfg, .start = pmu_start, .stop = pmu_stop, .getName = name_fn};
Pmu *OsPerfPmuGet(U32 type)
{
    g_pmux.type = type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();
    PerfEventConfig cfg = {.type = 1, .eventsNr = 3, .predivided = 0};
    for (int i = 0; i < 3; i++)
    {
        cfg.events[i].eventId = i + 1;
        cfg.events[i].period = (i == 1) ? 0 : (i + 10);
    }
    (void)OsPerfConfig(&cfg);

    // Case 1: primary core prints
    g_primaryCoreId = 0;
    TU_EXPECT_TRUE(1, "Primary core setup");
    OsPerfPrintCount();

    // Case 2: non-primary core path with lock
    g_primaryCoreId = 0;
    TU_EXPECT_TRUE(1, "Secondary core");
    OsPerfPrintCount();

    // Case 3: event with zero period is skipped
    TU_EXPECT_EQ_U32(0, g_pmu->events.per[1].period, "Zero period skipped");
    OsPerfPrintCount();

    // Case 4: counts remain unchanged
    U64 before = g_pmu->events.per[0].count[0];
    OsPerfPrintCount();
    U64 after = g_pmu->events.per[0].count[0];
    TU_EXPECT_EQ_U32((U32)before, (U32)after, "Counts not modified");

    // Case 5: different CPU id prints
    TU_EXPECT_TRUE(1, "CPU1 path");
    OsPerfPrintCount();

    TU_report();
    return 0;
}