#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "mock_include/prt_perf_output.h"
#include "mock_include/prt_atomic.h"
#include "mock_include/prt_stacktrace.h"

// -------- Global mocks --------
TickModInfo g_tickModInfo = {.tickPerSecond = 1000};
U32 g_systemClock = 1; // simplify time calc
U32 g_primaryCoreId = 0;

struct TagTskCb g_runningTask = {.taskPid = 1, .pc = 0, .fp = 0};

// cycle/time mocks
static U64 g_cycle = 0;
U64 OsCurCycleGet64(void) { return ++g_cycle; }
U64 PRT_ClkGetCycleCount64(void) { return ++g_cycle; }

// core and task mocks
static U32 g_coreId = 0;
U32 PRT_GetCoreID(void) { return g_coreId; }
__attribute__((weak)) U32 PRT_TaskSelf(TskHandle *threadId)
{
    if (threadId)
        *threadId = g_runningTask.taskPid;
    return OS_OK;
}

// light wrappers for safe funcs
U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count)
{
    if (!dest || !src || count > destMax)
        return OS_ERROR;
    (void)memcpy(dest, src, count);
    return OS_OK;
}
U32 memset_s(void *dest, size_t destMax, int c, size_t count)
{
    if (!dest || count > destMax)
        return OS_ERROR;
    (void)memset(dest, c, count);
    return OS_OK;
}
__attribute__((weak)) int PRT_Printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vprintf(fmt, ap);
    va_end(ap);
    return r;
}

// -------- PMU mocks --------
Pmu *g_pmu = NULL;
static U32 pmu_config_ok(void) { return OS_OK; }
static U32 pmu_start_ok(void) { return OS_OK; }
static U32 pmu_stop_ok(void) { return OS_OK; }
static const char *pmu_get_name(Event *e)
{
    (void)e;
    return "mock-event";
}

static Pmu g_mockPmu = {.events = {0}, .type = 0, .config = pmu_config_ok, .start = pmu_start_ok, .stop = pmu_stop_ok, .getName = pmu_get_name};

__attribute__((weak)) Pmu *OsPerfPmuGet(U32 type)
{
    g_mockPmu.type = type;
    return &g_mockPmu;
}

__attribute__((weak)) U32 OsHwPmuInit(void) { return OS_OK; }
__attribute__((weak)) U32 OsTimedPmuInit(void) { return OS_OK; }
__attribute__((weak)) U32 OsSwPmuInit(void) { return OS_OK; }

// -------- Output buffer mocks --------
static char *g_outBuf = NULL;
static U32 g_outCap = 0;
static U32 g_outLen = 0;
static U32 g_outRd = 0;
static PERF_BUF_NOTIFY_HOOK g_notify = NULL;
static PERF_BUF_FLUSH_HOOK g_flushHook = NULL;

__attribute__((weak)) U32 OsPerfOutPutInit(void *buf, U32 size)
{
    g_outBuf = (char *)buf;
    g_outCap = size;
    g_outLen = 0;
    g_outRd = 0;
    return OS_OK;
}
__attribute__((weak)) U32 OsPerfOutPutWrite(const char *data, U32 len)
{
    if (!g_outBuf || g_outLen + len > g_outCap)
        return OS_ERROR;
    memcpy(g_outBuf + g_outLen, data, len);
    g_outLen += len;
    if (g_notify)
        g_notify();
    return OS_OK;
}
__attribute__((weak)) U32 OsPerfOutPutRead(char *dest, U32 size)
{
    U32 remain = (g_outLen > g_outRd) ? (g_outLen - g_outRd) : 0;
    U32 n = size < remain ? size : remain;
    memcpy(dest, g_outBuf + g_outRd, n);
    g_outRd += n;
    return n;
}
void OsPerfOutPutFlush(void)
{
    if (g_flushHook)
        g_flushHook();
}
void OsPerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func) { g_notify = func; }
void OsPerfFlushHookReg(PERF_BUF_FLUSH_HOOK func) { g_flushHook = func; }

// -------- Stacktrace mock --------
U32 PRT_GetStackTraceByTaskID(U32 *depth, uintptr_t *callChain, U32 taskId)
{
    (void)taskId;
    if (!depth || !callChain)
        return OS_ERROR;
    U32 n = *depth;
    if (n > 3)
        n = 3;
    for (U32 i = 0; i < n; i++)
        callChain[i] = 0x1000 + i;
    *depth = n;
    return OS_OK;
}

// Helpers to control core id in tests
void test_set_core(U32 id) { g_coreId = id; }
