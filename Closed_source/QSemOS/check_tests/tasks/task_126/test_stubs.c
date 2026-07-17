/* Stub implementations for task_124 */

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