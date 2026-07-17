// Minimal mock header for prt_perf.c unit tests
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef uint32_t U32;
typedef uint64_t U64;
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

#define PERF_MAX_EVENT 8
#define PERF_MAX_FILTER_TSKS 16
#define PERF_MAX_CALLCHAIN_DEPTH 16

// Perf framework statuses
typedef enum
{
    PERF_UNINIT = 0,
    PERF_STOPED = 1,
    PERF_STARTED = 2,
} PerfStatus;

// PMU per-cpu status
#define PERF_PMU_STOPED 0U
#define PERF_PMU_STARTED 1U

// Sample type flags
#define PERF_RECORD_CPU (1U << 0)
#define PERF_RECORD_TID (1U << 1)
#define PERF_RECORD_TYPE (1U << 2)
#define PERF_RECORD_PERIOD (1U << 3)
#define PERF_RECORD_TIMESTAMP (1U << 4)
#define PERF_RECORD_IP (1U << 5)
#define PERF_RECORD_CALLCHAIN (1U << 6)

#define PERF_DATA_MAGIC_WORD 0xA55AF00DU

#ifndef OS_SEC_BSS
#define OS_SEC_BSS
#endif

// Tick module info
typedef struct
{
    U32 tickPerSecond;
} TickModInfo;

extern TickModInfo g_tickModInfo;
extern U32 g_systemClock;
extern U32 g_primaryCoreId;

// Task control block mock
struct TagTskCb
{
    U32 taskPid;
    uintptr_t pc;
    uintptr_t fp;
};

extern struct TagTskCb g_runningTask;
#define RUNNING_TASK (&g_runningTask)

// Event and PerfEvent
typedef struct
{
    U32 eventId;
    U32 period;
    U64 count[4];
} Event;

typedef struct
{
    Event per[PERF_MAX_EVENT];
    U32 nr;
    U32 cntDivided;
} PerfEvent;

// Perf sample data (must match write order in prt_perf.c)
typedef struct
{
    U32 cpuid;
    U32 taskId;
    U32 eventId;
    U32 period;
    U64 time;
    uintptr_t pc;
    struct
    {
        U32 ipNr;
        uintptr_t ip[PERF_MAX_CALLCHAIN_DEPTH];
    } callChain;
} PerfSampleData;

typedef struct
{
    U32 magic;
    U32 sampleType;
    U32 sectionId;
    U32 eventType;
    U32 len;
} PerfDataHdr;

typedef struct
{
    uintptr_t pc;
} PerfRegs;

typedef struct
{
    U32 eventId;
    U32 period;
} PerfEventItem;

typedef struct
{
    U32 type;
    U32 eventsNr;
    U32 predivided;
    PerfEventItem events[PERF_MAX_EVENT];
} PerfEventConfig;

typedef struct
{
    bool needStoreToBuffer;
    bool taskFilterEnable;
    U32 sampleType;
    U32 taskIdsNr;
    U32 taskIds[PERF_MAX_FILTER_TSKS];
    PerfEventConfig eventsCfg;
} PerfConfigAttr;

// Control block used by prt_perf.c
typedef struct
{
    U32 pmuStatusPerCpu[4];
    bool needStoreToBuffer;
    bool taskFilterEnable;
    U32 sampleType;
    U32 taskIdsNr;
    U32 taskIds[PERF_MAX_FILTER_TSKS];
    PerfStatus status;
    U64 startTime;
    U64 endTime;
} PerfCB;

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
