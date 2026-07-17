/* Minimal stub - memset_s provided by auto_stub.h */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
#include "common/shim_prt_task_test.h"

/* Definitions for globals declared in shim_prt_task_test.h */
PowerOffFuncT g_sysPowerOffHook = NULL;
SetOfflineFlagFuncT g_setOfflineFlagHook = NULL;
OsVoidFunc g_taskCoreSleep = NULL;
unsigned int g_timeSliceCycle = 3;
uintptr_t g_uniTicks = 0;
int g_calls_OsEnqueueTask = 0;
int g_calls_OsDequeueTask = 0;
int g_calls_OsIncNrRunning = 0;
int g_calls_OsDecNrRunning = 0;
int g_calls_OsReschedTask = 0;
int g_calls_OsReschedTaskNoWakeIpc = 0;
int g_last_sys_trace_pid = -1;
int g_calls_OsTskDlyNearestTicksRefresh = 0;

/* g_testDelayBase - point to the same delay list that auto_stub uses */
struct TagOsTskSortedDelayList *g_testDelayBase = &g_tskSortedDelay[0];