// Minimal mock header for prt_perf.c unit tests
#pragma once

// Undef conflicting macros from qsem_test_types.h before they cause problems
#ifdef OS_ERROR
#undef OS_ERROR
#endif
#ifdef PERF_MAX_FILTER_TSKS
#undef PERF_MAX_FILTER_TSKS
#endif
#ifdef PERF_PMU_STOPED
#undef PERF_PMU_STOPED
#endif
#ifdef PERF_STOPED
#undef PERF_STOPED
#endif
#ifdef PERF_STARTED
#undef PERF_STARTED
#endif
#ifdef PERF_UNINIT
#undef PERF_UNINIT
#endif
#ifdef PERF_STATUS_INIT
#undef PERF_STATUS_INIT
#endif
#ifdef PERF_RECORD_CPU
#undef PERF_RECORD_CPU
#endif
#ifdef PERF_RECORD_TID
#undef PERF_RECORD_TID
#endif
#ifdef PERF_RECORD_TYPE
#undef PERF_RECORD_TYPE
#endif
#ifdef PERF_RECORD_PERIOD
#undef PERF_RECORD_PERIOD
#endif
#ifdef PERF_RECORD_TIMESTAMP
#undef PERF_RECORD_TIMESTAMP
#endif
#ifdef PERF_RECORD_IP
#undef PERF_RECORD_IP
#endif
#ifdef PERF_RECORD_CALLCHAIN
#undef PERF_RECORD_CALLCHAIN
#endif
#ifdef PERF_DATA_MAGIC_WORD
#undef PERF_DATA_MAGIC_WORD
#endif
#ifdef PERF_MAX_EVENT
#undef PERF_MAX_EVENT
#endif
// Keep PERF_MAX_CALLCHAIN_DEPTH defined for src code

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

// Use same type definitions as src to avoid conflicts
typedef uint64_t U64;
typedef uint32_t U32;
typedef uint8_t U8;
typedef U32 TskHandle;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define OS_OK 0U
#define OS_ERROR 1U

#define OS_SYS_US_PER_SECOND 1000000ULL

// Perf max values - must match src
#define PERF_MAX_EVENT 7
#define PERF_MAX_FILTER_TSKS 32
#define PERF_MAX_CALLCHAIN_DEPTH 10

// PMU per-cpu status
#define PERF_PMU_STOPED 0U
#define PERF_PMU_STARTED 1U

// Perf framework statuses - use enum like src
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

#define PERF_DATA_MAGIC_WORD 0xEFEFEF00

#ifndef OS_SEC_BSS
#define OS_SEC_BSS
#endif

// Tick module info
#ifdef TickModInfo
#undef TickModInfo
#endif
typedef struct
{
    U32 tickPerSecond;
} TickModInfo;

#ifdef g_tickModInfo
#undef g_tickModInfo
#endif
extern TickModInfo g_tickModInfo;
extern U32 g_systemClock;
extern U32 g_primaryCoreId;

// Task control block mock
#ifdef TagTskCb
#undef TagTskCb
#endif
struct TagTskCb
{
    U32 taskPid;
    uintptr_t pc;
    uintptr_t fp;
};

#ifdef g_runningTask
#undef g_runningTask
#endif
extern struct TagTskCb g_runningTask;
#define RUNNING_TASK (&g_runningTask)

// Event structure
#ifdef Event
#undef Event
#endif
typedef struct {
    U32 eventId;
    U32 period;
    U64 count[4];
} Event;

// PerfEvent structure
#ifdef PerfEvent
#undef PerfEvent
#endif
typedef struct {
    Event per[PERF_MAX_EVENT];
    U32 nr;
    U32 cntDivided;
} PerfEvent;

// Pmu structure
#ifdef Pmu
#undef Pmu
#endif
typedef struct Pmu {
    PerfEvent events;
    U32 type;
    U32 (*config)(void);
    U32 (*start)(void);
    U32 (*stop)(void);
    const char *(*getName)(Event *e);
} Pmu;

// PerfRegs structure
#ifdef PerfRegs
#undef PerfRegs
#endif
typedef struct {
    uintptr_t pc;
} PerfRegs;

// PerfSampleData structure
#ifdef PerfSampleData
#undef PerfSampleData
#endif
typedef struct {
    U32 cpuid;
    U32 taskId;
    U32 eventId;
    U32 period;
    U64 time;
    uintptr_t pc;
    struct {
        U32 ipNr;
        uintptr_t ip[PERF_MAX_CALLCHAIN_DEPTH];
    } callChain;
} PerfSampleData;

// PerfDataHdr structure
#ifdef PerfDataHdr
#undef PerfDataHdr
#endif
typedef struct {
    U32 magic;
    U32 sampleType;
    U32 sectionId;
    U32 eventType;
    U32 len;
} PerfDataHdr;

// PerfCB structure
#ifdef PerfCB
#undef PerfCB
#endif
typedef struct {
    U32 pmuStatusPerCpu[4];
    bool needStoreToBuffer;
    bool taskFilterEnable;
    U32 sampleType;
    U32 taskIdsNr;
    U32 taskIds[PERF_MAX_FILTER_TSKS];
    enum PerfStatus status;
    U64 startTime;
    U64 endTime;
} PerfCB;

// Global PerfCB
extern PerfCB g_perfCb;
extern Pmu *g_pmu;

// PerfEventConfig - must match src
#ifdef PerfEventConfig
#undef PerfEventConfig
#endif
typedef struct {
    U32 type;
    struct {
        U32 eventId;
        U32 period;
    } events[PERF_MAX_EVENT];
    U32 eventsNr;
    U32 predivided;
} PerfEventConfig;

// PerfConfigAttr - must match src
#ifdef PerfConfigAttr
#undef PerfConfigAttr
#endif
typedef struct
{
    PerfEventConfig eventsCfg;
    U32 taskIds[PERF_MAX_FILTER_TSKS];
    U32 taskIdsNr;
    bool needStoreToBuffer;
    bool taskFilterEnable;
    U32 sampleType;
} PerfConfigAttr;

// Error codes used in prt_perf.c
#define OS_ERRNO_PERF_HW_INIT_ERROR 0x1001U
#define OS_ERRNO_PERF_TIMED_INIT_ERROR 0x1002U
#define OS_ERRNO_PERF_SW_INIT_ERROR 0x1003U
#define OS_ERRNO_PERF_INVALID_PMU 0x1004U
#define OS_ERRNO_PERF_PMU_CONFIG_ERROR 0x1005U
#define OS_ERRNO_PERF_CONFIG_NULL 0x1006U
#define OS_ERRNO_PERF_STATUS_INVALID 0x1007U
#define OS_ERRNO_PERF_BUF_ERROR 0x1008U

// Runtime APIs mocked in tests
U64 OsCurCycleGet64(void);
U64 PRT_ClkGetCycleCount64(void);
U32 PRT_GetCoreID(void);
U32 PRT_TaskSelf(TskHandle *threadId);
U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
U32 memset_s(void *dest, size_t destMax, int c, size_t count);
int PRT_Printf(const char *fmt, ...);

// SMP macro simplified for unit tests
#define SMP_CALL_PERF_FUNC(fn) fn()

// Spin lock for PERF_LOCK/PERF_UNLOCK
typedef U32 SplLock;
extern SplLock g_perfSpin;
U32 OsIntLock(void);
void OsIntRestore(U32 intSave);
U32 PRT_SplLockInit(SplLock *lock);
#define PERF_LOCK(intSave) intSave = OsIntLock()
#define PERF_UNLOCK(intSave) OsIntRestore(intSave)

// Function declarations for Perf init chain
U32 OsPmuInit(void);
U32 OsHwPmuInit(void);
U32 OsTimedPmuInit(void);
U32 OsSwPmuInit(void);

// Public API functions (implemented in test_stubs.c)
U32 PRT_PerfInit(void *buf, U32 size);
U32 PRT_PerfConfig(PerfConfigAttr *attr);
void PRT_PerfStart(U32 sectionId);
void PRT_PerfStop(void);
U32 PRT_PerfDataRead(char *dest, U32 size);
void PRT_PerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func);
void PRT_PerfFlushHookReg(PERF_BUF_FLUSH_HOOK func);

// Internal functions (implemented in test_stubs.c)
U32 OsPerfConfig(PerfEventConfig *eventsCfg);
void OsPerfStart(void);
void OsPerfStop(void);
void OsPerfPrintCount(void);
U32 OsPerfCollectData(Event *event, PerfSampleData *data, PerfRegs *regs);
// OsPerfTaskFilter is tested directly via func_under_test.c, not stubbed
bool OsPerfParamValid(void);
void OsPerfSetIrqRegs(uintptr_t pc, uintptr_t fp);
void OsPerfUpdateEventCount(Event *event, U32 value);
void OsPerfHandleOverFlow(Event *event, PerfRegs *regs);
U32 OsPerfHdrInit(U32 id);