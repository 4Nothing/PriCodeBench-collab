// Test-specific helpers for prt_perf.c unit tests
// This header should NOT define types that are already defined in source headers
// Only define test-specific macros, globals, and helper functions
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

// Task control block mock - must match struct TagTskCb from src
struct TagTskCb
{
    U32 taskPid;
    uintptr_t pc;
    uintptr_t fp;
};

extern struct TagTskCb g_runningTask;
#define RUNNING_TASK (&g_runningTask)

// Spinlock mock - simplified for unit tests
struct PrtSpinLock;
#define PRT_SplIrqLock(lock) 0
#define PRT_SplIrqUnlock(lock, state) do { (void)lock; (void)state; } while(0)
#define PRT_SplLockInit(lock) OS_OK

// Runtime APIs mocked in tests - use U32 for TskHandle to match source
U64 OsCurCycleGet64(void);
U64 PRT_ClkGetCycleCount64(void);
U32 PRT_GetCoreID(void);
U32 PRT_TaskSelf(U32 *threadId);
U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
U32 memset_s(void *dest, size_t destMax, int c, size_t count);
int PRT_Printf(const char *fmt, ...);
U32 PRT_GetStackTraceByTaskID(U32 *maxDepth, uintptr_t *callChain, U32 taskId);

// Output functions - to be mocked in tests or test_stubs.c
U32 OsPerfOutPutInit(void *buf, U32 size);
U32 OsPerfOutPutWrite(const char *data, U32 len);
U32 OsPerfOutPutRead(char *dest, U32 size);
void OsPerfOutPutFlush(void);
// OsPerfNotifyHookReg and OsPerfFlushHookReg are declared in source headers with correct types

// PMU functions - declared in source headers, do not redeclare here
// Pmu *OsPerfPmuGet(U32 type);  // declared in prt_perf_pmu.h
U32 OsHwPmuInit(void);
U32 OsTimedPmuInit(void);
U32 OsSwPmuInit(void);

// Tick module info
typedef struct
{
    U32 tickPerSecond;
} TickModInfo;

extern TickModInfo g_tickModInfo;
extern U32 g_systemClock;
extern U32 g_primaryCoreId;