/*
 * QSemOS Check Test Framework - Comprehensive Types
 * 
 * 提供测试所需的完整类型定义，支持所有 240 个任务。
 */

#ifndef __QSEM_TEST_TYPES_H__
#define __QSEM_TEST_TYPES_H__

/* Enable POSIX.199309 for struct sigaction and signal constants */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#ifndef __USE_POSIX199309
#define __USE_POSIX199309 1
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* System configuration */
#define OS_MAX_CORE_NUM 1

/* QSemOS signal types - define saHandler first, then struct _sigaction */
typedef void (*_sa_handler)(int signo);

/* QSemOS's _sigaction structure - maps to POSIX sigaction with sa_handler field */
#ifndef STRUCT_SIGACTION_DEFINED
#define STRUCT_SIGACTION_DEFINED
struct _sigaction {
    _sa_handler saHandler;
};
#endif

/* ============================================================
 * Basic types - match QSemOS definitions exactly
 * Use unsigned long long for U64/S64 to match src/include/uapi/prt_typedef.h
 * ============================================================ */
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char S8;
typedef signed short S16;
typedef signed int S32;
typedef signed long long S64;

/* Spinlock type - used early */
typedef volatile U32 SplLock;

/* PrtSpinLock type */
typedef struct PrtSpinLock {
    SplLock lock;
    uintptr_t rawLock;
} PrtSpinLock;

/* locale_t type */
typedef void *locale_t;

/* ============================================================
 * Handle types
 * ============================================================ */
typedef uint32_t TskHandle;
typedef uint32_t SemHandle;
typedef uint32_t TimerHandle;
typedef uint32_t HwiHandle;
typedef uint32_t QueueHandle;
typedef uint32_t ModHandle;
typedef uint32_t TimerGroupId;
typedef uint32_t TmrHandle;
typedef uint32_t TskPrior;
typedef uint32_t HwiArg;
typedef uint32_t HwiNum;
typedef uint32_t RwLockHandle;
typedef uint32_t SigSet;
typedef U64 (*SysTimeFunc)(void);

/* RwLock spinlock structure */
typedef struct {
    SplLock rwSpinLock;
} RwLockSpinLock;

/* List structure */
typedef struct TagListObject {
    struct TagListObject *next;
    struct TagListObject *prev;
} ListHead;

/* pthread_rwlock_t stub for compilation */
typedef struct {
    SplLock rwSpinLock;
    S32 rw_count;
    void *rw_owner;
    ListHead rw_read;
    ListHead rw_write;
    U32 rw_magic;
} pthread_rwlock_t;

/* RwLock structures for reader-writer lock */
#define RWLOCK_NONE_MODE 0
#define RWLOCK_READ_MODE 1
#define RWLOCK_WRITE_MODE 2
#define RWLOCK_WRITEFIRST_MODE 3
#define RWLOCK_READFIRST_MODE 4
#define RWLOCK_TRYRD 5
#define RWLOCK_TRYWR 6
#define RWLOCK_MAGIC_NUM 0xABCDEF00
#define RWLOCK_COUNT_MASK 0xFFFFFF

#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX - 1)

typedef struct TagRwLockCb {
    U32 rwCount;
    void *rwOwner;
    struct {
        struct TagListObject readList;
        struct TagListObject writeList;
    } rwList;
    SplLock rwSpinLock;
    U32 mode;
} TagRwLockCb;

/* ============================================================
 * Notifier structures
 * ============================================================ */
#define NOTIFY_DONE 0x0000U
#define NOTIFY_OK 0x0001U
#define NOTIFY_STOP_MASK 0x8000U
#define NOTIFY_REP_DONE 0x8000U

struct NotifierBlock;
typedef int (*NotifierFn)(struct NotifierBlock *self, unsigned long val, void *v);

typedef struct NotifierBlock {
    struct NotifierBlock *next;
    struct NotifierBlock *prev;
    NotifierFn call;  /* alias for notifierCall */
    NotifierFn notifierCall;
    int priority;
} NotifierBlock;

typedef struct NotifierHead {
    struct NotifierBlock head;
} NotifierHead;

/* ============================================================
 * Status/Return types
 * ============================================================ */
typedef uint32_t UssRet;
typedef uint32_t OsRet;

/* ============================================================
 * Error codes
 * ============================================================ */
#define OS_OK 0U
#define OS_ERROR (~0U)

/* HWI errors */
#define OS_ERRNO_HWI_UNCREATED 0x01000001U
#define OS_ERRNO_HWI_ID_INVALID 0x01000002U
#define OS_ERRNO_HWI_PTR_NULL 0x01000003U
#define OS_ERRNO_HWI_INIT_ERROR 0x01000004U
#define OS_ERRNO_HWI_NUM_INVALID 0x01000005U
#define OS_ERRNO_HWI_PROC_FUNC_NULL 0x01000006U
#define OS_HWI_MAX_NUM 64U

/* Task errors */
#define OS_ERRNO_TSK_ID_INVALID 0x02000001U
#define OS_ERRNO_TSK_PTR_NULL 0x02000002U
#define OS_ERRNO_TSK_NOT_CREATED 0x02000003U
#define OS_ERRNO_TSK_PRIOR_ERROR 0x02000004U
#define OS_ERRNO_TSK_OPERATE_IDLE 0x02000005U
#define OS_ERRNO_TSK_PRIORITY_INHERIT 0x02000006U
#define OS_ERRNO_TSK_PEND_MUTEX 0x02000007U
#define OS_ERRNO_TSK_PEND_PRIOR 0x02000008U
#define OS_ERRNO_TSK_PRIOR_LOWER_THAN_PENDTSK 0x02000009U

/* Semaphore errors */
#define OS_ERRNO_SEM_REG_ERROR 0x03000001U
#define OS_ERRNO_SEM_ID_INVALID 0x03000002U
#define OS_ERRNO_SEM_PTR_NULL 0x03000003U
#define OS_ERRNO_SEM_NOT_CREATED 0x03000004U
#define OS_ERRNO_SEM_ALL_BUSY 0x03000005U
#define OS_ERRNO_SEM_OVERFLOW 0x03000006U

/* Semaphore constants */
#define OS_SEM_COUNT_MAX 0xFFFFFFFFU
#define OS_SEM_FULL 1
#define SEM_TYPE_BIN 1
#define SEM_TYPE_COUNT 2
#define OS_SEM_UNUSED 0U
#define OS_SEM_USED 1
#define OS_SEM_INVALID 0U

/* Semaphore errors */
#define OS_ERRNO_SEM_UNAVAILABLE 0x03000007U
#define OS_ERRNO_SEM_PEND_IN_LOCK 0x03000008U
#define OS_ERRNO_SEM_INVALID 0x03000009U
#define OS_ERRNO_SEM_PEND_INTERR 0x0300000AU
#define OS_ERRNO_SEM_TIMEOUT 0x0300000BU

/* Timer errors */
#define OS_ERRNO_TIMER_INPUT_PTR_NULL 0x04000001U
#define OS_ERRNO_TIMER_INTERVAL_INVALID 0x04000002U
#define OS_ERRNO_TIMER_MODE_INVALID 0x04000003U
#define OS_ERRNO_TIMER_TYPE_INVALID 0x04000004U
#define OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED 0x04000005U
#define OS_ERRNO_TICK_NOT_INIT 0x04000006U
#define OS_ERRNO_TIMER_NUM_ZERO 0x04000007U
#define OS_ERRNO_TIMER_NUM_TOO_LARGE 0x04000008U
#define OS_ERRNO_TIMER_TICKGROUP_CREATE 0x04000009U
#define OS_ERRNO_TIMER_HANDLE_INVALID 0x0400000AU
#define OS_ERRNO_TIMER_TICKGROUP_CREATED 0x0400000BU
#define OS_ERRNO_SWTMR_MAXSIZE 0x0400000CU
#define OS_ERRNO_TIMER_PROC_FUNC_NULL 0x0400000DU
#define OS_ERRNO_TIMERGROUP_ID_INVALID 0x0400000EU
#define OS_ERRNO_SWTMR_INTERVAL_NOT_SUITED 0x0400000FU
#define OS_ERRNO_SWTMR_INTERVAL_OVERFLOW 0x04000010U
#define OS_ERRNO_SWTMR_NOT_CREATED 0x04000011U
#define OS_ERRNO_SWTMR_NO_MEMORY 0x04000012U
#define OS_ERRNO_SWTMR_UNSTART 0x04000013U
#define OS_TIMER_FREE 0
#define OS_TIMER_CREATED 1
#define OS_TIMER_EXPIRED 2
#define OS_TIMER_RUNNING 3
#define OS_SWTMR_STATUS_MASK 0xFFU
#define OS_SWTMR_PRE_FREE 0x10U
#define OS_SWTMR_PRE_RUNNING 0x20U
#define OS_SWTMR_PRE_CREATED 0x30U

/* Queue errors */
#define OS_ERRNO_QUEUE_ID_INVALID 0x05000001U
#define OS_ERRNO_QUEUE_PTR_NULL 0x05000002U
#define OS_ERRNO_QUEUE_NOT_CREATED 0x05000003U

/* Memory pool IDs */
#define OS_MID_PERF 0
#define OS_MID_SWTMR 1
#define OS_MEM_DEFAULT_PT0 0
#define OS_MEM_DEFAULT_FSC_PT 0
#define MEM_ADDR_ALIGN_016 16
#define EOK 0

/* ============================================================
 * Performance constants
 * ============================================================ */
#define PERF_BUFFER_WATERMARK_ONE_N 2
#define PERF_MAX_CALLCHAIN_DEPTH 16
#define PERF_RECORD_CPU 0x01
#define PERF_RECORD_TID 0x02
#define PERF_RECORD_TYPE 0x04
#define PERF_RECORD_PERIOD 0x08
#define PERF_RECORD_TIMESTAMP 0x10
#define PERF_RECORD_IP 0x20
#define PERF_RECORD_CALLCHAIN 0x40
#define PERF_COUNT_CPU_CLOCK 0U
#define PERF_STOPED 2U
#define PERF_STARTED 3U
#define PERF_UNINIT 0U
#define PERF_STATUS_INIT 1U
#define OS_ERRNO_PERF_STATUS_INVALID 0x09000001U
#define OS_ERRNO_PERF_BUF_ERROR 0x09000002U
#define OS_ERRNO_PERF_INVALID_PMU 0x09000003U
#define OS_ERRNO_PERF_PMU_CONFIG_ERROR 0x09000004U
#define OS_ERRNO_PERF_CONFIG_NULL 0x09000005U
#define PERF_MAX_FILTER_TSKS 16U

/* ============================================================
 * Performance structures
 * ============================================================ */
typedef struct {
    U32 startIdx;
    U32 endIdx;
    U32 size;
    U32 remain;
    PrtSpinLock lock;
    U32 status;
    char *fifo;
} Ringbuf;

typedef struct Event {
    U32 eventId;
    U32 period;
    U64 count[OS_MAX_CORE_NUM];
} Event;

typedef struct PerfRegs {
    uintptr_t pc;
    uintptr_t sp;
    uintptr_t fp;
} PerfRegs;

typedef struct PerfSampleData {
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

typedef struct PerfOutputCB {
    Ringbuf ringbuf;
    U32 waterMark;
} PerfOutputCB;

typedef struct PerfCB {
    U32 sampleType;
    U32 eventCount;
    U32 status;
    U32 *pmuStatusPerCpu;
    bool needStoreToBuffer;
    bool taskFilterEnable;
    U32 taskIdsNr;
    U32 *taskIds;
    U64 startTime;
    U64 endTime;
} PerfCB;
extern PerfCB g_perfCb;

/* Perf event structure */
typedef struct {
    U32 eventId;
    U32 period;
    U64 count[OS_MAX_CORE_NUM];
} PerfEventEntry;

typedef struct {
    U32 nr;
    PerfEventEntry *per;
    U32 enabled;
    U32 cntDivided;
} PerfEvent;

/* Perf config structure */
typedef struct {
    U32 type;
    U32 eventsNr;
    PerfEventEntry *events;
    U32 predisposed;
    U32 predivided;
    U32 predisabled;
    U32 predisved;
} PerfEventConfig;

/* Perf config attribute */
typedef struct {
    U32 eventType;
    U32 freq;
    U32 enabled;
    bool needStoreToBuffer;
    bool taskFilterEnable;
    U32 sampleType;
    U32 taskIdsNr;
    U32 *taskIds;
    PerfEventConfig eventsCfg;
} PerfConfigAttr;

/* Perf function types */
typedef void (*PerfEventHandler)(PerfEvent *event);
typedef void (*OsVoidFunc)(void);
typedef void (*PowerOffFuncT)(bool);
typedef void (*SetOfflineFlagFuncT)(bool);
extern PowerOffFuncT g_sysPowerOffHook;
extern bool g_sysPowerOffFlag;
extern SetOfflineFlagFuncT g_setOfflineFlagHook;

/* PMU related */
#define PERF_EVENT_TYPE_TIMED 1U
typedef struct {
    PerfEvent events;
    U32 type;
    U32 (*config)(void);
    U32 (*start)(void);
    U32 (*stop)(void);
    const char *(*getName)(const void *event);
} Pmu;
typedef struct {
    Pmu pmu;
} PerfTimed;
extern Pmu *g_pmu;
extern PerfTimed *g_perfTimed;
extern U32 g_swtPmuId;
extern SplLock g_perfSpin;

/* ============================================================
 * Section macros - strip for Linux
 * ============================================================ */
#define OS_SEC_BSS
#define OS_SEC_L4_TEXT
#define OS_SEC_L3_TEXT
#define OS_SEC_L2_TEXT
#define OS_SEC_L1_TEXT
#define OS_SEC_L0_TEXT
#define OS_SEC_TEXT
#define OS_SEC_ALW_INLINE inline __attribute__((always_inline))

/* ============================================================
 * Task constants
 * ============================================================ */
#define OS_TSK_PRIORITY_LOWEST 255U
#define OS_MAX_U16 65535U
#define OS_TSK_PEND 0x0008U
#define OS_TSK_READY 0x0004U
#define OS_TSK_TIMEOUT 0x0020U
#define OS_TSK_RW_PEND 0x1000U
#define OS_WAIT_FOREVER 0xFFFFFFFFU

/* ============================================================
 * Semaphore constants
 * ============================================================ */
#define OS_SEM_USED 1
#define OS_INVALID_OWNER_ID 0xFFFFFFFFU

/* Semaphore mode enum */
enum SemMode {
    SEM_MODE_FIFO = 0,
    SEM_MODE_PRIOR = 1
};

/* Semaphore module info */
typedef struct SemModInfo {
    U32 maxNum;
} SemModInfo;

/* ============================================================
 * Timer constants
 * ============================================================ */
#define TIMER_TYPE_INVALID 3
#define TIMER_TYPE_SWTMR 1
#define TIMER_TYPE_HWTMR 0
#define OS_TIMER_LOOP 0
#define OS_TIMER_ONCE 1
#define OS_TIMER_HARDWARE 0
#define OS_TIMER_SOFTWARE 1
#define OS_TIMER_GET_TYPE(handle) (((handle) >> 30) & 0x3)
#define OS_TICK_SWTMR_GROUP_ID 1

/* ============================================================
 * Timer structures
 * ============================================================ */
typedef struct TimerCreatePara {
    U32 type;
    U32 mode;
    U32 interval;
    U32 timerGroupId;
    void (*callback)(void);
    void (*callBackFunc)(void);
    void *arg;
    void *arg1;
    void *arg2;
    void *arg3;
    void *arg4;
} TimerCreatePara;

/* Timer group configuration */
typedef struct TmrGrpUserCfg {
    U32 tmrGrpSrcType;
    U32 maxTimerNum;
} TmrGrpUserCfg;

/* Timer group source types */
#define OS_TIMER_GRP_SRC_TICK 0
#define OS_TIMER_GRP_SRC_HWTMR 1

/* ============================================================
 * Signal constants
 * ============================================================ */
#define OS_SIGNAL_MAX 32
#define OS_MID_SIGNAL 0
#define OS_MEM_DEFAULT_FSC_PT 0
#define OS_ERRNO_SIGNAL_NO_MEMORY 0x06000001U
#define OS_ERRNO_SIGNAL_PARA_INVALID 0x06000002U
#define OS_ERRNO_SIGNAL_NUM_INVALID 0x06000003U
#define OS_ERRNO_SIGNAL_WAIT_IN_LOCK 0x06000004U
#define OS_ERRNO_SIGNAL_TIMEOUT 0x06000005U
#define OS_ERRNO_SIGNAL_TASKID_INVALID 0x06000006U
#define OS_ERRNO_SIGNAL_TSK_NOT_CREATED 0x06000007U

/* Signal wait forever constant */
#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFFU

/* Signal handler type */
typedef void (*_sa_handler)(int signo);
#ifndef OS_SIG_DEFAULT_HANDLER_DEFINED
#define OsSigDefaultHandler NULL
#endif

/* signalSet type alias */
typedef SigSet signalSet;

/* ============================================================
 * Signal structures
 * ============================================================ */
typedef struct signalInfo {
    int si_signo;
    int si_code;
    int si_value;
} signalInfo;

/* Forward declaration for sigInfoNode - full definition in stubs */
struct sigInfoNode;
typedef struct sigInfoNode sigInfoNode;

#define sigMask(signo) (1U << (signo))

/* ============================================================
 * HWI Macros
 * ============================================================ */
#define OS_IRQ2HWI(irq) ((HwiHandle)(irq))
#define OS_REPORT_ERROR(err) printf("Error: %u\n", (unsigned int)err)

/* HWI function types */
typedef void (*HwiProcFunc)(U32 arg);
typedef U32 HwiPrior;
typedef U32 HwiMode;
#define OS_HOOK_HWI_ENTRY 0x01U
#define OS_HOOK_HWI_EXIT 0x02U
#ifndef OS_HWI_DEFAULT_HANDLER_DEFINED
#define OsHwiDefaultHandler NULL
#endif

/* ============================================================
 * Atomic operations
 * ============================================================ */
#define OS_ATOMIC_CAS(ptr, old, new) __sync_bool_compare_and_swap(ptr, old, new)
#define OS_ATOMIC_ADD(ptr, val) __sync_add_and_fetch(ptr, val)
#define OS_ATOMIC_SUB(ptr, val) __sync_sub_and_fetch(ptr, val)
#define OS_ATOMIC_OR(ptr, val) __sync_or_and_fetch(ptr, val)
#define OS_ATOMIC_AND(ptr, val) __sync_and_and_fetch(ptr, val)
#define OS_ATOMIC_READ_U32(ptr) (*(ptr))
#define OS_ATOMIC_SET_U32(ptr, val) (*(ptr) = (val))

/* ============================================================
 * Common constants
 * ============================================================ */
#define U32_MAX 0xFFFFFFFFU
#define U16_MAX 0xFFFFU
#define U8_MAX 0xFFU
#define U12_INVALID 0xFFFU
#define U32_INVALID 0xFFFFFFFFU
#define OS_SYS_MS_PER_SECOND 1000U
#define OS_MAX_U16 65535U
#define OS_MAX_U32 0xFFFFFFFFU

/* System errors */
#define OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED 0x07000001U

/* ============================================================
 * Type limits
 * ============================================================ */
/* Option macros */
#define OS_OPTION_SMP 1
#define OS_OPTION_TICKLESS 1

/* RwLock modes */
#define RWLOCK_NONE_MODE 0
#define RWLOCK_READ_MODE 1
#define RWLOCK_WRITE_MODE 2
#define RWLOCK_WRITEFIRST_MODE 3
#define RWLOCK_READFIRST_MODE 4

/* Timer sort link */
#define OS_SWTMR_SORTLINK_LEN 32

/* Tickless constants */
#define OS_TICKLESS_FOREVER 0xFFFFFFFFU

/* Timer sort link attribute structure */
typedef struct TagSwTmrSortLinkAttr {
    struct TagListObject sortLink;
    U32 nearestTicks;
    SplLock spinLock;
} SwTmrSortLinkAttr;

/* ============================================================
 * Queue constants and types
 * ============================================================ */
/* Forward declaration for queue control block */
struct TagQueCb;

/* Queue error codes */
#define OS_ERRNO_QUEUE_NO_SOURCE 0x05000004U
#define OS_ERRNO_QUEUE_IN_INTERRUPT 0x05000005U
#define OS_ERRNO_QUEUE_INVALID 0x05000006U
#define OS_ERRNO_QUEUE_SIZE_ZERO 0x05000007U
#define OS_ERRNO_QUEUE_PRIO_INVALID 0x05000008U
#define OS_ERRNO_QUEUE_NOT_CREATE 0x05000009U
#define OS_ERRNO_QUEUE_NO_MEMORY 0x0500000AU
#define OS_ERRNO_QUEUE_CREAT_PTR_NULL 0x0500000BU
#define OS_ERRNO_QUEUE_PARA_ZERO 0x0500000CU
#define OS_ERRNO_QUEUE_NSIZE_INVALID 0x0500000DU
#define OS_ERRNO_QUEUE_MAXNUM_ZERO 0x0500000EU

/* Queue constants */
#define OS_QUEUE_NO_WAIT 0U
#define OS_QUEUE_NORMAL 0U
#define OS_QUEUE_URGENT 1U
#define OS_QUEUE_UNUSED 0U
#define OS_QUEUE_NODE_HEAD_LEN 8U
#define OS_QUEUE_NODE_SIZE_ALIGN 8U
#define OS_QUEUE_WAIT_FOREVER OS_WAIT_FOREVER

/* Queue errors */
#define OS_ERRNO_QUEUE_PEND_IN_LOCK 0x0500000FU
#define OS_ERRNO_QUEUE_TIMEOUT 0x05000010U
#define OS_ERRNO_QUEUE_SIZE_TOO_BIG 0x05000011U
#define OS_ERRNO_QUEUE_CB_UNAVAILABLE 0x05000012U

/* Queue module ID */
#define OS_MID_QUEUE 2

/* ============================================================
 * Task constants and types
 * ============================================================ */
/* Forward declarations */
struct TagTskCb;
struct TskInitParam;
struct TskModInfo;
struct TskInfo;

/* Task error codes */
#define OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK 0x0200000AU
#define OS_ERRNO_TSK_YIELD_INVALID_TASK 0x0200000BU
#define OS_ERRNO_TSK_ENTRY_NULL 0x0200000CU
#define OS_ERRNO_TSK_MAX_NUM_NOT_SUITED 0x0200000DU
#define OS_ERRNO_BUILD_ID_INVALID 0x0200000EU

/* Task constants */
#define OS_TSK_SUSPEND 0x0010U
#define OS_TSK_INUSE 0x0020U
#define OS_TSK_NAME_LEN 32U
#define OS_TSK_PARA_0 0U
#define OS_TSK_PARA_1 4U
#define OS_TSK_PARA_2 8U
#define OS_TSK_PARA_3 12U
#define OS_TSK_STACK_SIZE_ALLOC_ALIGN 8U
#define OS_TSK_STACK_SIZE_ALIGN 8U
#define OS_TSK_STACK_MAGIC 0xDECADEADU
#define OS_TSK_STACK_TOP_MAGIC 0x12345678U
#define OS_TSK_NULL_ID 0U
#define OS_TSK_RUNNING 0x0002U
#define OS_TSK_QUEUE_PEND 0x0800U
#define OS_TSK_QUEUE_BUSY 0x0400U
#define OS_TSK_SUSPEND_READY_BLOCK 0x0040U
#define OS_TSK_STKSZ_NOT_ALIGN 0x0200000FU
#define OS_TSK_STACKADDR_NOT_ALIGN 0x02000010U
#define MAX_TASK_NUM 1024U
#define OS_TSK_DEFAULT_TIME_SLICE_MS 10U
#define OS_SYS_MS_PER_SECOND 1000U
#define OS_TSK_SCHED_RR 1U

/* System flag constants */
extern U32 TICK_NO_RESPOND_CNT;
#define OS_FLG_TICK_ACTIVE 0x04U
#define OS_FLG_TSK_REQ 0x10U
#define OS_ERRNO_TSK_ACTIVE_FAILED 0x02000011U
#define OS_ERRNO_TSK_DELAY_IN_INT 0x02000012U
#define OS_ERRNO_TSK_DELAY_IN_LOCK 0x02000013U
#define OS_ERRNO_TSK_UNLOCK_NO_LOCK 0x02000014U

/* Task module ID */
#define OS_MID_TSK 3

/* Hook types */
#define OS_HOOK_TSK_CREATE 0x01U
#define OS_HOOK_FIRST_TIME_SWH 0x02U
#define OS_FLG_BGD_ACTIVE 0x01U
#define OS_FLG_TSK_SWHK 0x02U

/* ============================================================
 * HWI/Interrupt constants
 * ============================================================ */
#define OS_HWI_ACTIVE 1U
#define OS_INT_ACTIVE 1U
#define OS_HWI_ACTIVE_MASK 0x01U
#define OS_HWI_HANDLE 1U
typedef U32 OS_TASK_LOCK_DATA;  /* legacy typedef, use g_uniTaskLock directly */
#define OS_TASK_LOCK_DATA_VAR   g_uniTaskLock

/* ============================================================
 * Signal/Locale constants
 * ============================================================ */
/* locale_t is defined in locale.h on Linux, use void* as fallback */
#ifdef __linux__
#include <locale.h>
#else
#define locale_t void*
#endif
typedef void* LocaleT;
enum OsLogLevel {
    OS_LOG_EMERG = 0U,
    OS_LOG_ALERT,
    OS_LOG_CRIT,
    OS_LOG_ERR,
    OS_LOG_WARNING,
    OS_LOG_NOTICE,
    OS_LOG_INFO,
    OS_LOG_DEBUG,
    OS_LOG_NONE
};
typedef enum OsLogLevel OsLogLevel;

enum OsLogFacility {
    OS_LOG_F0 = 0x01U,
    OS_LOG_F1 = 0x02U,
    OS_LOG_F2 = 0x04U,
    OS_LOG_F3 = 0x08U,
    OS_LOG_F4 = 0x10U,
    OS_LOG_F5 = 0x20U,
    OS_LOG_F6 = 0x40U,
    OS_LOG_F7 = 0x80U
};
typedef enum OsLogFacility OsLogFacility;

#define LOG_FACILITY_NUM 4U

/* ============================================================
 * Module/Dynamic loading constants and types
 * ============================================================ */
#include <elf.h>

/* Module error codes */
#define OS_MODULE_OK 0U
#define OS_MODULE_ERRNO_MEMORY_ALLOC 0x08000001U
#define OS_MAX_MODULE_NUM 32U
#define OS_MID_DYNAMIC 4U
#define OS_MODULE_ERROR_STR_LEN 64U
#define MODULE_UNIT_INIT 0U

/* ELF types for Linux */
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Sword Elf_Sword;
typedef Elf64_Word Elf_Word;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Rel Elf_Rel;

#define ELF_R_SYM(info) (ELF64_R_SYM(info))
#define ELF_R_TYPE(info) (ELF64_R_TYPE(info))
#define ELF_ST_BIND(info) (ELF64_ST_BIND(info))

struct OsDynModuleRelocInfo {
    uintptr_t reloc;
    U32 relocType;
    U32 shType;
    Elf_Addr symAddr;
};

/* Dynamic module info structure */
struct DynSymbol {
    char *name;
    void *addr;
};

struct DynModuleUnitInfo {
    U32 valid;
    U32 modId;
    char name[32];
    U32 baseAddr;
    U32 size;
    char *moduleStr;
    struct DynSymbol *symTab;
    U32 symNum;
    void *loadSegMem;
    U32 unitNo;
    U32 state;
    U32 loadSegStartAddr;
    U32 loadSegEndAddr;
    U32 stDev;
    U32 stIno;
};

/* ============================================================
 * Global variables (declarations)
 * ============================================================ */
extern U32 g_maxQueue;
extern struct TagQueCb *g_allQueue;
typedef void (*TaskScanHook)(void);
extern TaskScanHook g_taskScanHook;
extern struct DynModuleUnitInfo **g_dynModuleInfoPool;
extern struct TagListObject g_tskCbFreeList;
extern struct TskModInfo g_tskModInfo;
extern U32 g_primaryCoreId;
extern U32 g_numOfCores;
extern U64 g_timeSliceCycle;
extern U32 g_systemClock;
typedef void (*TskEntryFunc)(U32 arg);
extern TskEntryFunc g_tskIdleEntry;

/* UNI_FLAG and system flags */
extern U32 UNI_FLAG;
#define OS_FLG_BGD_ACTIVE 0x01U

/* Log system variables */
extern void *g_logMemBase;
extern U32 g_logOn;
extern U32 g_logFilter[8];
extern PrtSpinLock g_logLock;
#define HEAD_PTR_OFFSET 0U
#define TAIL_PTR_OFFSET sizeof(void*)
#define BUFFER_BLOCK_SIZE 256U
#define BUFFER_BLOCK_NUM 16U
#define LOG_MAX_SIZE 1024U
#define VALID_FLAGS_OFFSET 0U
#define CLOCK_REALTIME 0

struct logHeader {
    U32 sequenceNum;
    U32 level;
    U32 facility;
    U32 sec;
    U32 nanoSec;
    U32 taskPid;
    U32 len;
};

extern U32 g_sequenceNum;

/* DynModule error string */
extern char *g_dynModuleErrorStr;

/* Idle hook types */
#define OS_HOOK_IDLE_PERIOD 0x01U
#define OS_HOOK_IDLE_PREFIX 0x02U
extern OsVoidFunc g_taskCoreSleep;
extern void (*coreSleep)(void);

/* Signal wait error */
#define OS_ERRNO_SIGNAL_WAIT_NOT_IN_TASK 0x06000002U

/* Global task locks */
extern struct TagTskCb *g_highestTask;
extern U32 g_uniTaskLock;

/* Perf timed handle */
typedef U32 (*OsPerfTimedHandle)(PerfEvent *event, U32 timeout);
extern OsPerfTimedHandle g_perfTimedHandle;

/* RBUF status */
#define RBUF_INITED 1U

/* Spl alloc error */
#define OS_ERRNO_SPL_ALLOC_ADDR_INVALID 0x0A000001U

#endif /* __QSEM_TEST_TYPES_H__ */
