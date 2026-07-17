#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "mock_include/prt_perf_output.h"
#include "test_util.h"
#include "func_under_test.c"

static int g_write_called = 0;
U32 OsPerfOutPutInit(void *b, U32 s)
{
    (void)b;
    (void)s;
    return OS_OK;
}
U32 OsPerfOutPutWrite(const char *d, U32 l)
{
    (void)d;
    (void)l;
    g_write_called++;
    return OS_OK;
}
U32 OsPerfOutPutRead(char *d, U32 s)
{
    (void)d;
    (void)s;
    return 0;
}

static const char *name_fn(Event *e)
{
    (void)e;
    return "evt";
}
static U32 pmu_cfg(void) { return OS_OK; }
static U32 pmu_start(void) { return OS_OK; }
static U32 pmu_stop(void) { return OS_OK; }
static Pmu g_pmux = {.events = {{0}}, .type = 2, .config = pmu_cfg, .start = pmu_start, .stop = pmu_stop, .getName = name_fn};
Pmu *OsPerfPmuGet(U32 type)
{
    (void)type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();
    // Prepare config
    PerfConfigAttr attr = {0};
    attr.needStoreToBuffer = true;
    attr.sampleType = 0;
    attr.eventsCfg.type = 2;
    attr.eventsCfg.eventsNr = 1;
    attr.eventsCfg.events[0].eventId = 1;
    attr.eventsCfg.events[0].period = 1;

    // 1) status invalid
    g_perfCb.status = PERF_STARTED;
    PRT_PerfStart(10);
    TU_EXPECT_EQ_U32(PERF_STARTED, g_perfCb.status, "Invalid status");

    // 2) not configured -> param invalid
    g_perfCb.status = PERF_STOPED;
    g_pmu = NULL;
    PRT_PerfStart(11);
    TU_EXPECT_EQ_U32(PERF_STOPED, g_perfCb.status, "Not configured");

    // 3) configured and buffer mode -> header write called
    g_write_called = 0;
    g_perfCb.status = PERF_STOPED;
    (void)PRT_PerfConfig(&attr);
    PRT_PerfStart(12);
    TU_EXPECT_EQ_U32(1, g_write_called, "Header write called");

    // 4) status becomes STARTED
    TU_EXPECT_EQ_U32(PERF_STARTED, g_perfCb.status, "Status started");

    // 5) start time recorded
    TU_EXPECT_TRUE(g_perfCb.startTime != 0, "Start time");

    TU_report();
    return 0;
}