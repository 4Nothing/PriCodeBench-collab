#include "test_util.h"
#include "mock_include/prt_perf.h"
extern U32 OsPerfBackTrace(uintptr_t *buf, U32 depth, PerfRegs *regs);
#include "func_under_test.c"

static const char *name_fn(Event *e)
{
    (void)e;
    return "evt";
}
static U32 pmu_cfg(void) { return OS_OK; }
static U32 pmu_start(void) { return OS_OK; }
static U32 pmu_stop(void) { return OS_OK; }
static Pmu g_pmux = {
    .type = 0,
    .events = {.nr = 0, .cntDivided = 0},
    .config = pmu_cfg,
    .start = pmu_start,
    .stop = pmu_stop,
    .getName = name_fn
};
Pmu *OsPerfPmuGet(U32 type)
{
    g_pmux.type = type;
    return &g_pmux;
}

int main(void)
{
    TU_reset();
    PerfRegs r = {.pc = 0xABCDEF00};
    Event ev = {.eventId = 42, .period = 7};
    PerfSampleData data;
    memset(&data, 0, sizeof(data));

    /* 1) Only CPU */
    g_perfCb.sampleType = PERF_RECORD_CPU;
    U32 sz = OsPerfCollectData(&ev, &data, &r);
    TU_EXPECT_EQ_U32(sizeof(U32), sz, "Size with CPU only");

    /* 2) CPU + TID + TYPE */
    memset(&data, 0, sizeof(data));
    g_perfCb.sampleType = PERF_RECORD_CPU | PERF_RECORD_TID | PERF_RECORD_TYPE;
    sz = OsPerfCollectData(&ev, &data, &r);
    TU_EXPECT_EQ_U32(sizeof(U32) * 3, sz, "Size with CPU+TID+TYPE");

    /* 3) PERIOD + TIMESTAMP */
    memset(&data, 0, sizeof(data));
    g_perfCb.sampleType = PERF_RECORD_PERIOD | PERF_RECORD_TIMESTAMP;
    sz = OsPerfCollectData(&ev, &data, &r);
    TU_EXPECT_EQ_U32(4 + 8, sz, "Size with PERIOD+TIMESTAMP");

    /* 4) IP only */
    memset(&data, 0, sizeof(data));
    g_perfCb.sampleType = PERF_RECORD_IP;
    sz = OsPerfCollectData(&ev, &data, &r);
    TU_EXPECT_EQ_U32(sizeof(uintptr_t), sz, "Size with IP only");

    /* 5) CALLCHAIN: ipNr (U32) + max depth * sizeof(uintptr_t) */
    memset(&data, 0, sizeof(data));
    g_perfCb.sampleType = PERF_RECORD_CALLCHAIN;
    sz = OsPerfCollectData(&ev, &data, &r);
    TU_EXPECT_EQ_U32(sizeof(U32) + PERF_MAX_CALLCHAIN_DEPTH * sizeof(uintptr_t), sz, "Size with callchain");

    TU_report();
    return 0;
}
