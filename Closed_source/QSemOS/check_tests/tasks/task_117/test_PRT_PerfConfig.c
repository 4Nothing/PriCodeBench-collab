#include <stddef.h>
void *memset(void *s, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
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
    PerfConfigAttr attr;
    memset(&attr, 0, sizeof(attr));
    attr.needStoreToBuffer = true;
    attr.taskFilterEnable = true;
    attr.sampleType = 0xAA55;
    attr.taskIdsNr = 3;
    attr.taskIds[0] = 11;
    attr.taskIds[1] = 22;
    attr.taskIds[2] = 33;
    attr.eventsCfg.type = 1;
    attr.eventsCfg.eventsNr = 1;
    attr.eventsCfg.events[0].eventId = 7;
    attr.eventsCfg.events[0].period = 5;

    // 1) null attr
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_CONFIG_NULL, PRT_PerfConfig(NULL), "Null attr");

    // 2) status invalid (not STOPED)
    g_perfCb.status = PERF_UNINIT;
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_STATUS_INVALID, PRT_PerfConfig(&attr), "Status invalid");

    // 3) status STOPED -> OK
    g_perfCb.status = PERF_STOPED;
    TU_EXPECT_EQ_U32(OS_OK, PRT_PerfConfig(&attr), "Config OK");

    // 4) fields copied
    TU_EXPECT_EQ_U32(3, g_perfCb.taskIdsNr, "Task count copied");
    TU_EXPECT_EQ_U32(11, g_perfCb.taskIds[0], "Task id[0] copied");

    // 5) needStoreToBuffer copied
    TU_EXPECT_TRUE(g_perfCb.needStoreToBuffer, "Buffer flag copied");

    TU_report();
    return 0;
}