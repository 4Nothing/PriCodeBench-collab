#include "mock_include/prt_perf.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

Pmu *g_pmu = NULL;

U32 memset_s(void *dest, size_t destMax, int c, size_t count) {
    memset(dest, c, count < destMax ? count : destMax);
    return 0;
}

int PRT_Printf(const char *fmt, ...) { return 0; }

/* Declaration before including func_under_test.c */
Pmu *OsPerfPmuGet(U32 type);

#include "func_under_test.c"

static int TU_total = 0;
static int TU_pass = 0;

static void TU_reset(void) { TU_total = 0; TU_pass = 0; }

static void TU_EXPECT_EQ_U32(U32 expected, U32 actual, const char *name) {
    ++TU_total;
    int ok = (expected == actual);
    if (ok) ++TU_pass;
    printf("[%s] expect=%u actual=%u -> %s\n", name, expected, actual, ok ? "PASS" : "FAIL");
}

static void TU_report(void) {
    double rate = TU_total ? (100.0 * (double)TU_pass / (double)TU_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, TU_pass, TU_total);
}

static U32 g_cfg_ret = OS_OK;
static const char *name_fn(Event *e) { (void)e; return "evt"; }
static U32 pmu_config_custom(void) { return g_cfg_ret; }
static U32 pmu_start(void) { return OS_OK; }
static U32 pmu_stop(void) { return OS_OK; }
static Pmu g_pmux = {{0}, 0, pmu_config_custom, pmu_start, pmu_stop, name_fn};

Pmu *OsPerfPmuGet(U32 type) {
    if (type == 0xFFFFFFFF) return NULL;
    g_pmux.type = type;
    return &g_pmux;
}

static PerfEventConfig make_cfg(U32 type, U32 nr) {
    PerfEventConfig c = {.type = type, .eventsNr = nr, .predivided = 7};
    for (U32 i = 0; i < nr && i < PERF_MAX_EVENT; i++) {
        c.events[i].eventId = 100 + i;
        c.events[i].period = i + 1;
    }
    return c;
}

int main(void) {
    TU_reset();
    PerfEventConfig cfg;

    cfg = make_cfg(0xFFFFFFFF, 2);
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_INVALID_PMU, OsPerfConfig(&cfg), "Invalid PMU type");

    g_cfg_ret = OS_OK;
    cfg = make_cfg(1, 3);
    TU_EXPECT_EQ_U32(OS_OK, OsPerfConfig(&cfg), "Config OK");

    TU_EXPECT_EQ_U32(3, g_pmu->events.nr, "Event number copied");
    TU_EXPECT_EQ_U32(7, g_pmu->events.cntDivided, "Predivided copied");

    g_cfg_ret = 0x1234;
    cfg = make_cfg(2, 1);
    TU_EXPECT_EQ_U32(OS_ERRNO_PERF_PMU_CONFIG_ERROR, OsPerfConfig(&cfg), "Config fails");

    cfg = make_cfg(3, PERF_MAX_EVENT + 5);
    g_cfg_ret = OS_OK;
    TU_EXPECT_EQ_U32(OS_OK, OsPerfConfig(&cfg), "Events capped OK");

    TU_report();
    return TU_pass == TU_total ? 0 : 1;
}