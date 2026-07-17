// Mock header for prt_perf.c unit tests
// This header defines ALL types needed for testing prt_perf.c
// It should NOT include source headers to avoid conflicts
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Basic type definitions - must match src/include/uapi/prt_typedef.h
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char S8;
typedef signed short S16;
typedef signed int S32;
typedef signed long long S64;
typedef U32 TskHandle;

// Task control block mock - must have pc and fp fields for OsPerfSetIrqRegs
struct TagTskCb {
    U32 taskPid;
    uintptr_t pc;
    uintptr_t fp;
};

extern struct TagTskCb g_runningTask;
#define RUNNING_TASK (&g_runningTask)

// SMP support - simplified for unit tests
#ifndef OS_OPTION_SMP
#define OS_OPTION_SMP 0
#endif
#ifndef OS_MAX_CORE_NUM
#define OS_MAX_CORE_NUM 1
#endif
#define PRT_KERNEL_CORE_NUM 1
#define SMP_CALL_PERF_FUNC(func) func()

// Perf max values - must match src/include/uapi/prt_perf.h
#define PERF_MAX_EVENT 7
#define PERF_MAX_FILTER_TSKS 32
#define PERF_MAX_CALLCHAIN_DEPTH 10

// PMU per-cpu status - must match src enum PmuStatus
enum PmuStatus {
    PERF_PMU_STOPED,
    PERF_PMU_STARTED,
};

// Perf framework statuses - must match src enum PerfStatus
enum PerfStatus {
    PERF_UNINIT,
    PERF_STARTED,
    PERF_STOPED,
};

// PerfEventType - must match src
typedef enum {
    PERF_EVENT_TYPE_HW,
    PERF_EVENT_TYPE_TIMED,
    PERF_EVENT_TYPE_SW,
    PERF_EVENT_TYPE_RAW,
    PERF_EVENT_TYPE_MAX
} PerfEventType;

// PerfSampleType - must match src
typedef enum {
    PERF_RECORD_CPU       = 1U << 0,
    PERF_RECORD_TID       = 1U << 1,
    PERF_RECORD_TYPE      = 1U << 2,
    PERF_RECORD_PERIOD    = 1U << 3,
    PERF_RECORD_TIMESTAMP = 1U << 4,
    PERF_RECORD_IP        = 1U << 5,
    PERF_RECORD_CALLCHAIN = 1U << 6,
} PerfSampleType;

// Buffer notify hook type
typedef void (*PERF_BUF_NOTIFY_HOOK)(void);

// Buffer flush hook type
typedef void (*PERF_BUF_FLUSH_HOOK)(void *addr, U32 size);

// PerfRegs structure - must match src
typedef struct {
    uintptr_t pc;
    uintptr_t fp;
} PerfRegs;

// PerfBackTrace structure - must match src
typedef struct {
    uintptr_t ipNr;
    uintptr_t ip[PERF_MAX_CALLCHAIN_DEPTH];
} PerfBackTrace;

// PerfSampleData structure - must match src
typedef struct {
    U32           cpuid;
    U32           taskId;
    U32           eventId;
    U32           period;
    U64           time;
    uintptr_t     pc;
    PerfBackTrace callChain;
} PerfSampleData;

// PerfDataHdr structure - must match src
typedef struct {
    U32             magic;
    PerfEventType   eventType;
    U32             len;
    PerfSampleType  sampleType;
    U32             sectionId;
} PerfDataHdr;

// Event structure - must match src
typedef struct {
    U32 counter;
    U32 eventId;
    U32 period;
    U64 count[PRT_KERNEL_CORE_NUM];
} Event;

// PerfEvent structure - must match src
typedef struct {
    Event per[PERF_MAX_EVENT];
    U8 nr;
    U8 cntDivided;
} PerfEvent;

// Pmu structure - must match src
typedef struct {
    PerfEventType type;
    PerfEvent events;
    U32 (*config)(void);
    U32 (*start)(void);
    U32 (*stop)(void);
    char *(*getName)(Event *event);
} Pmu;

// PerfCB structure - must match src
typedef struct {
    U64                  startTime;
    U64                  endTime;
    enum PerfStatus      status;
    enum PmuStatus       pmuStatusPerCpu[PRT_KERNEL_CORE_NUM];
    PerfSampleType       sampleType;
    U32                  taskIds[PERF_MAX_FILTER_TSKS];
    U8                   taskIdsNr;
    bool                 needStoreToBuffer;
    bool                 taskFilterEnable;
} PerfCB;

// PerfEventConfig - must match src
typedef struct {
    PerfEventType type;
    struct {
        U32 eventId;
        U32 period;
    } events[PERF_MAX_EVENT];
    U32 eventsNr;
    bool predivided;
} PerfEventConfig;

// PerfConfigAttr - must match src
typedef struct {
    PerfEventConfig      eventsCfg;
    U32                  taskIds[PERF_MAX_FILTER_TSKS];
    U32                  taskIdsNr;
    PerfSampleType       sampleType;
    bool                 needStoreToBuffer;
    bool                 taskFilterEnable;
} PerfConfigAttr;

// Tick module info
typedef struct {
    U32 tickPerSecond;
} TickModInfo;

extern TickModInfo g_tickModInfo;
extern U32 g_systemClock;
extern U32 g_primaryCoreId;

// Global PerfCB and PMU - extern declarations
extern PerfCB g_perfCb;
extern Pmu *g_pmu;

// Error codes used in prt_perf.c - must match src
#define OS_MID_PERF 0x17
#define OS_ERRNO_BUILD_ERROR(mid, err) (((mid) << 8) | (err))

#define OS_ERRNO_PERF_STATUS_INVALID        OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x00)
#define OS_ERRNO_PERF_HW_INIT_ERROR         OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x01)
#define OS_ERRNO_PERF_TIMED_INIT_ERROR      OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x02)
#define OS_ERRNO_PERF_SW_INIT_ERROR         OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x03)
#define OS_ERRNO_PERF_BUF_ERROR             OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x04)
#define OS_ERRNO_PERF_INVALID_PMU           OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x05)
#define OS_ERRNO_PERF_PMU_CONFIG_ERROR      OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x06)
#define OS_ERRNO_PERF_CONFIG_NULL           OS_ERRNO_BUILD_ERROR(OS_MID_PERF, 0x07)

#define OS_OK 0U
#define OS_ERROR 1U

// Spinlock mock - simplified for unit tests
struct PrtSpinLock;
#define PRT_SplIrqLock(lock) 0
#define PRT_SplIrqUnlock(lock, state) do { (void)lock; (void)state; } while(0)
#define PRT_SplLockInit(lock) OS_OK

// Runtime APIs mocked in tests
U64 OsCurCycleGet64(void);
U64 PRT_ClkGetCycleCount64(void);
U32 PRT_GetCoreID(void);
U32 PRT_TaskSelf(TskHandle *threadId);
U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
U32 memset_s(void *dest, size_t destMax, int c, size_t count);
int PRT_Printf(const char *fmt, ...);
U32 PRT_GetStackTraceByTaskID(U32 *maxDepth, uintptr_t *callChain, U32 taskId);

// Output functions - to be mocked in tests or test_stubs.c
U32 OsPerfOutPutInit(void *buf, U32 size);
U32 OsPerfOutPutWrite(char *data, U32 len);
U32 OsPerfOutPutRead(char *dest, U32 size);
void OsPerfOutPutFlush(void);
void OsPerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func);
void OsPerfFlushHookReg(PERF_BUF_FLUSH_HOOK func);

// PMU functions - to be mocked in tests
Pmu *OsPerfPmuGet(U32 type);
U32 OsHwPmuInit(void);
U32 OsTimedPmuInit(void);
U32 OsSwPmuInit(void);

// Public API functions (implemented in prt_perf.c)
// Do NOT declare these here - they will be declared in source headers
// U32 PRT_PerfInit(void *buf, U32 size);
// U32 PRT_PerfConfig(PerfConfigAttr *attr);
// void PRT_PerfStart(U32 sectionId);
// void PRT_PerfStop(void);
// U32 PRT_PerfDataRead(char *dest, U32 size);
// void PRT_PerfNotifyHookReg(const PERF_BUF_NOTIFY_HOOK func);
// void PRT_PerfFlushHookReg(const PERF_BUF_FLUSH_HOOK func);

// Internal functions that are NOT static in source
// Do NOT declare these here - they will be declared in source headers
// void OsPerfSetIrqRegs(uintptr_t pc, uintptr_t fp);
// void OsPerfUpdateEventCount(Event *event, U32 value);
// void OsPerfHandleOverFlow(Event *event, PerfRegs *regs);

// Macros to prevent qsem_test_stubs.h from defining conflicting weak functions
#define OS_PERF_PARAM_VALID_DEFINED
#define OS_PERF_HDR_INIT_DEFINED
#define OS_PERF_START_DEFINED