/*
 * Auto-generated stub header for prt_timer.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/core/kernel/timer/prt_timer.c
 * Included headers: 8
 */
#ifndef AUTO_STUB_PRT_TIMER_H
#define AUTO_STUB_PRT_TIMER_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char S8;
typedef signed short S16;
typedef signed int S32;
typedef signed long long S64;
typedef void *VirtAddr;
typedef void *PhyAddr;
/* ---- Types ---- */
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
/* From:  */
enum MoudleId {
    OS_MID_SYS = 0x0, /* 系统模块 */
    OS_MID_MEM = 0x1, /* 内存模块 */
    OS_MID_FSCMEM = 0x2,
    OS_MID_TSK = 0x3,
    OS_MID_SWTMR = 0x4,
    OS_MID_TICK = 0x5,
    OS_MID_CPUP = 0x6,
    OS_MID_SEM = 0x7,
    OS_MID_HWI = 0x8,
    OS_MID_HOOK = 0x9,
    OS_MID_EXC = 0xa,
    OS_MID_EVENT = 0xb,
    OS_MID_QUEUE = 0xc,
    OS_MID_TIMER = 0xd,
    OS_MID_HARDDRV = 0xe,
    OS_MID_APP = 0xf,
    OS_MID_SIGNAL = 0x10,
    OS_MID_SHELL = 0x11,
    OS_MID_SCHED = 0X12,
    OS_MID_SPL = 0X13,
    OS_MID_DYNAMIC = 0x14,
    OS_MID_LOG = 0x15,
    OS_MID_STACKTRACE = 0x16,
    OS_MID_PERF = 0x17,
    OS_MID_BUTT
};
enum TimerFlag {
    OS_TIMER_FREE = 1, /* 定时器空闲状态 */
    OS_TIMER_CREATED = 2, /* 定时器没有运行 */
    OS_TIMER_RUNNING = 4, /* 定时器正在运行 */
    OS_TIMER_EXPIRED = 8 /* 定时器已经超时 */
};
enum TimerGrpSrcType {
    OS_TIMER_GRP_SRC_HARDWARE, /* 硬件私有时钟源 */
    OS_TIMER_GRP_SRC_HARDWARE_SHARED, /* 硬件共享时钟源 */
    OS_TIMER_GRP_SRC_EXTERN, /* 外部时钟源 */
    OS_TIMER_GRP_SRC_TICK, /* TICK时钟源 */
    OS_TIMER_GRP_SRC_INVALID
};
enum TmrMode {
    OS_TIMER_LOOP, /* 定时器周期触发模式 */
    OS_TIMER_ONCE, /* 定时器单次触发模式 */
    OS_TIMER_INVALID_MODE
};
enum TimerType {
    OS_TIMER_HARDWARE, /* 硬件定时器(核内私有硬件定时器) */
    OS_TIMER_SOFTWARE, /* 软件定时器(核内私有软件定时器) */
    OS_TIMER_SOFTWARE_SHARED, /* 共享软件定时器，目前不支持 */
    OS_TIMER_INVALID_TYPE
};
/* From: src/include/uapi/prt_timer.h */
typedef U32 TimerHandle;
/* From: src/include/uapi/prt_err.h */
typedef void (*ErrHandleFunc)(const char *fileName, U32 lineNo, U32 errorNo, U32 paraLen, void *para);
/* From: src/include/uapi/prt_timer.h */
typedef U32 TimerGroupId;
struct TmrGrpUserCfg {
    /* 时钟源类型 */
    enum TimerGrpSrcType tmrGrpSrcType;
    /* 时间轮的刻度，即每刻度多少个us */
    U32 perStep;
    /* 最大定时器个数 */
    U32 maxTimerNum;
    /* 时间轮长度必须是2的N次方，此处配置N的大小 */
    U32 wheelLen2Power;
};
/* From: src/core/kernel/include/prt_timer_external.h */
typedef U32(*TimerRestartFunc)(TimerHandle timerHdl);
/* From: src/include/uapi/prt_timer.h */
typedef void (*TmrProcFunc)(TimerHandle tmrHandle, U32 arg1, U32 arg2, U32 arg3, U32 arg4);
/* From: src/core/kernel/include/prt_timer_external.h */
typedef U32(*TimerStopFunc)(TimerHandle timerHdl);
typedef U32(*TimerStartFunc)(TimerHandle timerHdl);
typedef U32(*TimerSetIntervalFunc)(TimerHandle timerHdl, U32 interVal);
typedef U32(*TimerDeleteFunc)(TimerHandle timerHdl);
typedef U32(*TimerQueryFunc)(TimerHandle timerHdl, U32 *expireTime);
typedef U32(*TimerGetOverrunFunc)(TimerHandle timerHdl, U32 *overrun);
/* From: src/include/uapi/prt_timer.h */
struct TimerCreatePara {
    /* 定时器创建模块ID,当前未使用，忽略 */
    U32 moduleId;
    /* 定时器类型 */
    enum TimerType type;
    /* 定时器工作模式 */
    enum TmrMode mode;
    /* 定时器周期(单次指定时器响应时长)，软件定时器单位是ms，硬件定时器单位是us，高精度定时器单位是ns */
    U32 interval;
    /*
     * 定时器组号，硬件定时器不使用，若创建软件定时器，定时器组ID由OS创建
     */
    U32 timerGroupId;
    /* 定时器硬中断优先级 */
    U16 hwiPrio;
    U16 resv;
    /*
     * 定时器回调函数
     */
    TmrProcFunc callBackFunc;
    /* 定时器用户参数1 */
    U32 arg1;
    /* 定时器用户参数2 */
    U32 arg2;
    /* 定时器用户参数3 */
    U32 arg3;
    /* 定时器用户参数4 */
    U32 arg4;
};
struct SwTmrInfo {
    /* 定时器状态，三种状态:Free,Created,Running,Expired */
    U8 state;
    /* 保留字段 */
    U8 resved[3];
    /* 定时器类型，两种类型:周期性、一次性 */
    enum TmrMode mode;
    /* 定时器超时间隔 */
    U32 interval;
    /* 定时器离超时剩余的ms数 */
    U32 remainMs;
    /* 定时器超时处理函数 */
    TmrProcFunc handler;
};
/* From: src/core/kernel/include/prt_timer_external.h */
typedef U32(*TimerCreateFunc)(struct TimerCreatePara *createPara, TimerHandle *tmrHandle);
struct TagFuncsLibTimer {
    TimerCreateFunc createTimer;
    TimerStartFunc startTimer;
    TimerStopFunc stopTimer;
    TimerDeleteFunc deleteTimer;
    TimerRestartFunc restartTimer;
    TimerSetIntervalFunc setIntervalTimer;
    TimerQueryFunc timerQuery;
    TimerGetOverrunFunc getOverrun;
};
/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))
/* From: src/include/uapi/prt_errno.h */
#define ERRNO_OS_ID (0x00U << 16)
#define ERRTYPE_ERROR (0x02U << 24)
#define ERRTYPE_FATAL (0x03U << 24)
#define ERRTYPE_NORMAL (0x00U << 24)
#define ERRTYPE_WARN (0x01U << 24)
/* From: src/include/uapi/prt_typedef.h */
#define FALSE ((bool)0)
#define INLINE static __inline __attribute__((always_inline))
#define LIKELY(x) __builtin_expect(!!(x), 1)
/* From: src/om/include/prt_err_external.h */
#define LOG_ADDR_DBG(addr) ((void)(addr), 0x0U)
/* From: src/include/uapi/prt_typedef.h */
#define NO 0
#define NULL ((void *)0)
#define OS_EMBED_ASM __asm__ __volatile__
/* From: src/include/uapi/prt_errno.h */
#define OS_ERRNO_BUILD_ERROR(mid, errno) (ERRTYPE_ERROR | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
#define OS_ERRNO_BUILD_FATAL(mid, errno) (ERRTYPE_FATAL | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
/* From: src/include/uapi/prt_timer.h */
#define OS_ERRNO_SWTMR_INTERVAL_NOT_SUITED OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x09)
#define OS_ERRNO_SWTMR_INTERVAL_OVERFLOW OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x0f)
#define OS_ERRNO_SWTMR_MAXSIZE OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x0a)
#define OS_ERRNO_SWTMR_NOT_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x0b)
#define OS_ERRNO_SWTMR_NO_MEMORY OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x0d)
#define OS_ERRNO_SWTMR_RET_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x13)
#define OS_ERRNO_SWTMR_UNSTART OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x0c)
#define OS_ERRNO_TICK_NOT_INIT OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x0e)
#define OS_ERRNO_TIMERGROUP_ID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x11)
#define OS_ERRNO_TIMER_HANDLE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x03)
#define OS_ERRNO_TIMER_INPUT_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x01)
#define OS_ERRNO_TIMER_INTERVAL_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x04)
#define OS_ERRNO_TIMER_MODE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x05)
#define OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x08)
#define OS_ERRNO_TIMER_NUM_TOO_LARGE OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x12)
#define OS_ERRNO_TIMER_NUM_ZERO OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x07)
#define OS_ERRNO_TIMER_PROC_FUNC_NULL OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x02)
#define OS_ERRNO_TIMER_TICKGROUP_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x10)
#define OS_ERRNO_TIMER_TYPE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TIMER, 0x06)
/* From: src/include/uapi/prt_typedef.h */
#define OS_ERROR (U32)(-1)
/* From: src/om/include/prt_err_external.h */
#define OS_ERROR_LOG_REPORT(traceLevel, format, ...) 
#define OS_ERROR_TYPE_MASK (0xffU << 24)
#define OS_ERROR_TYPE_NUM (ERRTYPE_FATAL >> 24) /* 中断中需要延后处理的错误类型(所有低于FATAL等级的类型)数 */
#define OS_ERR_LEVEL_HIGH 0
#define OS_ERR_LEVEL_LOW 2
#define OS_ERR_MAGIC_WORD 0xa1b2d4f8
#define OS_ERR_RECORD(errorNo) 
#define OS_ERR_RECORD_N 3  // 表示保留二进制低n位，计算OS_ERR_RECORD_NUM用，OS_ERR_RECORD_NUM为2的n次方+1
#define OS_ERR_RECORD_NUM
/* From: src/include/uapi/prt_typedef.h */
#define OS_FAIL 1
#define OS_INVALID (-1)
/* From: src/om/include/prt_err_external.h */
#define OS_LOG_LEVEL_FORCE 0xFF
#define OS_LOG_REPORT_DBG(format, ...) 
/* From: src/include/uapi/prt_typedef.h */
#define OS_OK 0
/* From: src/om/include/prt_err_external.h */
#define OS_REPORT_ERROR(errNo) 
/* From: (build-generated) */
#define OS_SEC_ALIGN
#define OS_SEC_ALW_INLINE
#define OS_SEC_BSS
#define OS_SEC_DATA
#define OS_SEC_L0_TEXT
#define OS_SEC_L1_TEXT
#define OS_SEC_L2_TEXT
#define OS_SEC_L3_TEXT
#define OS_SEC_L4_BSS
#define OS_SEC_L4_DATA
#define OS_SEC_L4_TEXT
#define OS_SEC_RO
#define OS_SEC_RW
#define OS_SEC_TEXT
#define OS_SEC_WEAK
#define OS_SEC_ZI
/* From: src/core/kernel/include/prt_timer_external.h */
#define OS_SWTMR_PRE_CREATED 0x80 /* 在软件定时器超时状态下进行停止操作，定时器进入预created态 */
#define OS_SWTMR_PRE_FREE 0x40 /* 在软件定时器超时状态下进行删除操作，定时器进入预free态 */
#define OS_SWTMR_PRE_RUNNING 0xc0 /* 在软件定时器超时状态下进行删除操作，定时器进入预running态 */
#define OS_TIMER_GET_HANDLE(type, index) (((type) << 28) | (index))
#define OS_TIMER_GET_INDEX(handle) ((handle) & 0x0FFFFFFFU)
#define OS_TIMER_GET_TYPE(handle) ((handle) >> 28)
#define OS_TIMER_PRE_CREATED (OS_SWTMR_PRE_CREATED | (U8)OS_TIMER_EXPIRED)
#define OS_TIMER_PRE_FREE (OS_SWTMR_PRE_FREE | (U8)OS_TIMER_EXPIRED)
#define OS_TIMER_PRE_RUNNING (OS_SWTMR_PRE_RUNNING | (U8)OS_TIMER_EXPIRED)
/* From: src/arch/include/prt_attr_external.h */
#define PRT_ATTR_EXTERNAL_H
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/om/include/prt_err_external.h */
#define PRT_ERR_EXTERNAL_H
/* From: src/include/uapi/prt_err.h */
#define PRT_ERR_H
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/core/kernel/include/prt_timer_external.h */
#define PRT_TIMER_EXTERNAL_H
/* From: src/include/uapi/prt_timer.h */
#define PRT_TIMER_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
/* From: src/core/kernel/include/prt_timer_external.h */
#define TIMER_TYPE_HWTMR 0
#define TIMER_TYPE_SWTMR 1
/* From: src/include/uapi/prt_typedef.h */
#define TRUE ((bool)1)
#define TRUNCATE(addr, size) ((addr) & ~((uintptr_t)(size) - 1))
#define U12_INVALID 0xfffU
#define U16_INVALID 0xffffU
#define U32_INVALID 0xffffffffU
#define U32_MAX 0xFFFFFFFFU
#define U64_INVALID 0xffffffffffffffffUL
#define U8_INVALID 0xffU
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define YES 1
/* ---- Common runtime shims ---- */
/* Secure memset (used by QSemOS kernel code) */
static inline int memset_s(void *dest, size_t destsz, int ch, size_t count)
{
    if (dest == NULL) return (int)('E' | ('N' << 8) | ('O' << 16) | ('S' << 24));
    if (count > destsz) return (int)('E' | ('R' << 8) | ('N' << 16) | ('O' << 24));
    (void)memset(dest, ch, count);
    return 0;
}
/* ---- Runtime function stubs (kernel functions called by sibling code) ---- */
struct TagTskCb;
struct TagSemCb;
struct TagScheduleClass;
typedef U16 TskPrior;
typedef U16 SemHandle;
/* Interrupt lock / restore */
static inline uintptr_t PRT_HwiLock(void) { return 0; }
static inline void PRT_HwiRestore(uintptr_t intSave) { (void)intSave; }
/* Task runqueue spinlock */
static inline void OsSpinLockTaskRq(struct TagTskCb *tcb) { (void)tcb; }
static inline void OsSpinUnlockTaskRq(struct TagTskCb *tcb) { (void)tcb; }
/* Ready list operations */
static inline void OsTskReadyDel(struct TagTskCb *tcb) { (void)tcb; }
static inline void OsTskReadyAdd(struct TagTskCb *tcb) { (void)tcb; }
/* Scheduler trigger */
static inline void OsTskSchedule(void) { }
/* Semaphore priority lock */
static inline void OsSemPrioLock(void) { }
static inline void OsSemPrioUnLock(void) { }
/* Check priority set (g_checkPrioritySet validation hook) */
static inline U32 OsCheckPrioritySet(struct TagTskCb *tcb, TskPrior prio) { (void)tcb; (void)prio; return 0; }
/* Task lock/unlock */
static inline void OsTskLock(void) { }
static inline void OsTskUnlock(void) { }
/* Memory allocation shim */
static inline void *PRT_MemAlloc(U32 pool, U32 pt, U32 size)
{
    (void)pool; (void)pt;
    return malloc(size);
}
static inline void PRT_MemFree(U32 pool, void *ptr)
{
    (void)pool;
    free(ptr);
}
/* ---- Common globals (stubs for kernel-BSS variables) ---- */
static U32 g_tskBaseId = 0;
static U32 g_tskMaxNum = 0;
static U32 g_idleTaskId = 0;
static U32 g_maxNumOfCores = 1;
static U32 g_uniTaskLock = 0;
static U32 g_tickNoRespondCnt = 0;
static U32 g_createTskLock = 0;
static void *g_tskCbArray = NULL;
static char g_runQueue[256];  /* placeholder */
static char g_tskSortedDelay[128];  /* placeholder */
static void *g_runningTask = NULL;
static void *g_semTsk = NULL;
static void *g_tskSem = NULL;
static char g_osRtSingleSchedClass[64];  /* placeholder */
#endif /* AUTO_STUB_PRT_TIMER_H */
