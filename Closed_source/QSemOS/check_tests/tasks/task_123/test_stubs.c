/* Stub implementations for task_123 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "shim_prt_task_test.h"

// Definitions for globals declared extern in shim_prt_task_test.h
PowerOffFuncT g_sysPowerOffHook = NULL;
SetOfflineFlagFuncT g_setOfflineFlagHook = NULL;
OsVoidFunc g_taskCoreSleep = NULL;
unsigned int g_timeSliceCycle = 3;
uintptr_t g_uniTicks = 0;

// Instrumentation globals
int g_calls_OsEnqueueTask = 0;
int g_calls_OsDequeueTask = 0;
int g_calls_OsIncNrRunning = 0;
int g_calls_OsDecNrRunning = 0;
int g_calls_OsReschedTask = 0;
int g_calls_OsReschedTaskNoWakeIpc = 0;
int g_last_sys_trace_pid = -1;
int g_calls_OsTskDlyNearestTicksRefresh = 0;

// Run queue array - must match definition in shim_prt_task_test.h
struct TagOsRunQue g_runq[OS_MAX_CORE_NUM];

// Delay list
struct TagOsTskSortedDelayList g_testDelayBase;

// OsIdleTaskExe - call g_taskCoreSleep if set
// This overrides the weak definition in qsem_test_stubs.h
// SKIP when func_under_test.c provides the implementation
#define SKIP_OsIdleTaskExe
#ifndef SKIP_OsIdleTaskExe
void OsIdleTaskExe(void)
{
    if (g_taskCoreSleep != NULL) {
        g_taskCoreSleep();
    }
}
#endif

// Test harness functions
void test_reset_stats(void)
{
    g_calls_OsEnqueueTask = 0;
    g_calls_OsDequeueTask = 0;
    g_calls_OsIncNrRunning = 0;
    g_calls_OsDecNrRunning = 0;
    g_calls_OsReschedTask = 0;
    g_calls_OsReschedTaskNoWakeIpc = 0;
    g_last_sys_trace_pid = -1;
    g_calls_OsTskDlyNearestTicksRefresh = 0;
    g_runq[0].nrRunning = 0;
    g_runq[0].tskCurr = NULL;
    ListInit(&g_testDelayBase.tskList);
}

int g_expect_failures = 0;
int g_expect_passes = 0;

void EXPECT_INT_EQ(const char *test_name, int expected, int actual)
{
    if (expected == actual) {
        g_expect_passes++;
        printf("PASS: %s (expected %d, got %d)\n", test_name, expected, actual);
    } else {
        g_expect_failures++;
        printf("FAIL: %s (expected %d, got %d)\n", test_name, expected, actual);
    }
}

void test_print_summary(void)
{
    printf("\n=== Test Summary ===\n");
    printf("Passes: %d\n", g_expect_passes);
    printf("Failures: %d\n", g_expect_failures);
    if (g_expect_failures == 0) {
        printf("All tests passed!\n");
    }
}