#include "mock_include/prt_perf.h"
#include "test_util.h"
#include "mock_include/prt_perf_pmu.h"
#include "prt_perf_output.h"

static char buf[1024];
static int writes = 0;
static int flushes = 0;

// Mock implementations for output functions
U32 OsPerfOutPutInit(void *b, U32 s) { return (b && s) ? OS_OK : OS_ERROR; }
U32 OsPerfOutPutWrite(const char *d, U32 l)
{
    (void)d;
    (void)l;
    writes++;
    return OS_OK;
}
void OsPerfOutPutFlush(void) { flushes++; }
U32 OsPerfOutPutRead(char *d, U32 s)
{
    (void)d;
    (void)s;
    return 0;
}

// Mock PMU
static const char *name_fn(Event *e)
{
    (void)e;
    return "evt";
}
static U32 pmu_cfg(void) { return OS_OK; }
static U32 pmu_start(void) { return OS_OK; }
static U32 pmu_stop(void) { return OS_OK; }
static Pmu g_pmux = {.events = {{0}}, .type = 5, .config = pmu_cfg, .start = pmu_start, .stop = pmu_stop, .getName = name_fn};
Pmu *OsPerfPmuGet(U32 type)
{
    (void)type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();
    // Init system
    TU_EXPECT_EQ_U32(OS_OK, PRT_PerfInit(buf, sizeof(buf)), "Init OK");

    // Configure buffer mode with one event and task filter off
    PerfConfigAttr attr = {0};
    attr.needStoreToBuffer = true;
    attr.taskFilterEnable = false;
    attr.sampleType = PERF_RECORD_CPU | PERF_RECORD_TID;
    attr.eventsCfg.type = 5;
    attr.eventsCfg.eventsNr = 1;
    attr.eventsCfg.events[0].eventId = 1;
    attr.eventsCfg.events[0].period = 10;
    TU_EXPECT_EQ_U32(OS_OK, PRT_PerfConfig(&attr), "Config OK");

    // Start/Stop
    writes = flushes = 0;
    PRT_PerfStart(99);
    TU_EXPECT_TRUE(g_perfCb.status == PERF_STARTED, "Started");
    PRT_PerfStop();
    TU_EXPECT_TRUE(g_perfCb.status == PERF_STOPED, "Stoped");
    TU_EXPECT_TRUE(flushes >= 1, "Flushed after stop");

    // Start again to ensure idempotence
    PRT_PerfStart(100);
    TU_EXPECT_TRUE(g_perfCb.status == PERF_STARTED, "Restarted");

    PRT_PerfStop();
    TU_EXPECT_TRUE(g_perfCb.endTime >= g_perfCb.startTime, "Times recorded");

    TU_report();
    return 0;
}