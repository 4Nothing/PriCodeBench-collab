/*
 * Auto-generated stub header for prt_log.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/utility/log/prt_log.c
 * Included headers: 12
 */
#ifndef AUTO_STUB_PRT_LOG_H
#define AUTO_STUB_PRT_LOG_H
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
/* ---- Structural macros (used in type definitions) ---- */
/* From: src/include/uapi/prt_buildef.h */
#define OS_OPTION_SMP
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_NAME_LEN 16
/* ---- Types ---- */
/* From: src/include/uapi/prt_hwi.h */
typedef uintptr_t HwiArg;
/* From: src/include/uapi/prt_sys.h */
typedef void (*PrtIdleHook)(void);
/* From: src/include/uapi/prt_task.h */
typedef void (*TskEntryFunc)(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
/* From: src/include/uapi/prt_atomic.h */
struct OsSpinLock
{
    volatile uintptr_t rawLock;
};
struct PrtSpinLock
{
    volatile uintptr_t rawLock;
};
/* From:  */
enum HookType {
    OS_HOOK_HWI_ENTRY = 0,  // 硬中断进入钩子
    OS_HOOK_HWI_EXIT,       // 硬中断退出钩子
    OS_HOOK_TSK_SWITCH,     // 任务切换钩子
    OS_HOOK_IDLE_PERIOD,    // IDLE钩子
    OS_HOOK_LAST_WORDS,     // 临终遗言钩子
    OS_HOOK_TSK_CREATE,     //任务创建钩子
    OS_HOOK_TSK_DELETE,     //任务切换钩子
    OS_HOOK_TYPE_NUM,       // 钩子总数
};
enum OsHwiIpiType {
    OS_TYPE_TRIGGER_BY_MASK = 0, /* 通过mask确定需要触发的目标核 */
    OS_TYPE_TRIGGER_TO_OTHER, /* 触发除本核外的其他核 */
    OS_TYPE_TRIGGER_TO_SELF, /* 触发本核 */
    OS_TYPE_TRIGGER_BUTT /* 非法 */
};
enum OsLogLevel {
    OS_LOG_EMERG = 0,
    OS_LOG_ALERT,
    OS_LOG_CRIT,
    OS_LOG_ERR,
    OS_LOG_WARN,
    OS_LOG_NOTICE,
    OS_LOG_INFO,
    OS_LOG_DEBUG,
    OS_LOG_NONE,    /* 仅用于过滤, 并非实际的日志级别 */
};
enum OsLogFacility {
    OS_LOG_F0 = 16,
    OS_LOG_F1,
    OS_LOG_F2,
    OS_LOG_F3,
    OS_LOG_F4,
    OS_LOG_F5,
    OS_LOG_F6,
    OS_LOG_F7,
};
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
enum SysThreadType {
    SYS_HWI,      /* 当前线程类型属于硬中断 */
    SYS_TASK,     /* 当前线程类型属于任务 */
    SYS_KERNEL,   /* 当前线程类型属于内核 */
    SYS_BUTT
};
enum SysRndNumType {
    OS_SYS_RND_STACK_PROTECT,     /* 栈保护的RND值 */
    OS_SYS_RND_BUTT               /* 非法类型 */
};
/* From: src/include/uapi/prt_hwi.h */
typedef void (*HwiProcFunc)(HwiArg);
/* From: src/include/uapi/prt_task.h */
typedef U16 TskStatus;
/* From: src/include/uapi/prt_hwi.h */
typedef U16 HwiPrior;
typedef U16 HwiMode;
/* From: src/include/uapi/prt_task.h */
typedef U16 TskPrior;
/* From: src/include/uapi/prt_sys.h */
typedef U32 (*CoreWakeUpHook)(U32 coreId);
/* From: src/include/uapi/prt_hook.h */
struct HookModInfo {
    U8 maxNum[(U32)OS_HOOK_TYPE_NUM];
};
/* From: src/include/uapi/prt_hwi.h */
typedef void (*HwiExitHook)(U32 hwiNum);
/* From: src/include/uapi/prt_task.h */
struct TskStackInfo {
    /* 栈顶 */
    uintptr_t top;
    /* 栈底 */
    uintptr_t bottom;
    /* 栈当前SP指针值 */
    uintptr_t sp;
    /* 栈当前使用的大小 */
    U32 currUsed;
    /* 栈使用的历史峰值 */
    U32 peakUsed;
    /* 栈是否溢出 */
    bool ovf;
};
/* From: src/include/uapi/prt_hwi.h */
typedef void (*HwiEntryHook)(U32 hwiNum);
typedef U32 HwiHandle;
/* From: src/include/uapi/hw/armv7-m/prt_task.h */
struct TskContext {
    U32 r4;
    U32 r5;
    U32 r6;
    U32 r7;
    U32 r8;
    U32 r9;
    U32 r10;
    U32 r11;
    U32 basePri;
    U32 excReturn;
    /*
     * 在有保存FPU寄存器，
     * 需要用这个顺序来看内存
     */
    U32 s16;
    U32 s17;
    U32 s18;
    U32 s19;
    U32 s20;
    U32 s21;
    U32 s22;
    U32 s23;
    U32 s24;
    U32 s25;
    U32 s26;
    U32 s27;
    U32 s28;
    U32 s29;
    U32 s30;
    U32 s31;
    U32 r0;
    U32 r1;
    U32 r2;
    U32 r3;
    U32 r12;
    U32 lr;
    U32 pc;
    U32 xpsr;
};
/* From: src/include/uapi/prt_task.h */
typedef U32 TskHandle;
struct TskModInfo {
    /* 最大支持的任务数 */
    U32 maxNum;
    /* 缺省的任务栈大小 */
    U32 defaultSize;
    /* Idle任务的任务栈大小 */
    U32 idleStackSize;
    /* 任务栈初始化魔术字 */
    U32 magicWord;
    /* 时间片调度，时间片粒度，单位ms */
    U32 timeSliceMs;
};
/* From: src/include/uapi/prt_sys.h */
typedef U64 (*SysTimeFunc)(void);
/* From: src/utility/log/prt_log_internal.h */
struct logHeader {
    U64 sec;
    U64 nanoSec;
    U64 sequenceNum;
    U32 taskPid;
    U16 len;
    U8 facility;
    U8 level;
    U8 logContent[];
};
/* From: src/include/uapi/prt_task.h */
struct TskInitParam {
    /* 任务入口函数 */
    TskEntryFunc taskEntry;
    /* 任务优先级 */
    TskPrior taskPrio;
    U16 reserved;
    /* 任务参数，最多4个 */
    uintptr_t args[4];
    /* 任务栈的大小 */
    U32 stackSize;
    /* 任务名 */
    char *name;
    /*
     * 本任务的任务栈独立配置起始地址，用户必须对该成员进行初始化，
     * 若配置为0表示从系统内部空间分配，否则用户指定栈起始地址
     */
    uintptr_t stackAddr;
    /* 任务调度策略 */
    U16 policy;
};
struct TskInfo {
    /* 任务切换时的SP */
    uintptr_t sp;
    /* 任务切换时的PC */
    uintptr_t pc;
    /* 任务状态 */
    TskStatus taskStatus;
    /* 任务优先级 */
    TskPrior taskPrio;
    /* 任务栈的大小 */
    U32 stackSize;
    /* 任务栈的栈顶 */
    uintptr_t topOfStack;
    /* 任务名 */
    char name[OS_TSK_NAME_LEN];
    /* 任务当前核 */
    U32 core;
    /* 任务入口函数 */
    void *entry;
    /* 任务控制块地址 */
    void *tcbAddr;
    /* 栈底 */
    uintptr_t bottom;
    /* 栈当前使用的大小 */
    U32 currUsed;
    /* 栈使用的历史峰值 */
    U32 peakUsed;
    /* 栈是否溢出 */
    bool ovf;
    /* 任务上下文 */
    struct TskContext context;
};
typedef U32(*TskSwitchHook)(TskHandle lastTaskPid, TskHandle nextTaskPid);
typedef U32(*TskDeleteHook)(TskHandle taskPid);
typedef U32(*TskCreateHook)(TskHandle taskPid);
/* From: src/include/uapi/prt_sys.h */
struct SysModInfo {
    /* CPU主频，时钟周期 */
    U32 systemClock;
    /* 用户注册的获取系统时间函数 */
    SysTimeFunc sysTimeHook;
    /* CPU type */
    U32 cpuType;
#if defined(OS_OPTION_SMP)
    /* 实际运行的核数 */
    U8 coreRunNum;
    /* 最大支持的核数*/
    U8 coreMaxNum;
    /* 主核ID */
    U8 corePrimary;
#endif
#if defined(OS_OPTION_HWI_MAX_NUM_CONFIG)
#endif
    U32 hwiMaxNum;
};
/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))
/* From: src/utility/log/prt_log_internal.h */
#define BUFFER_BLOCK_NUM 16U
#define BUFFER_BLOCK_SIZE 256U /* 1KB */
/* From: src/include/uapi/prt_task.h */
#define COMPOSE_PID(coreid, handle) 
/* From: src/include/uapi/prt_errno.h */
#define ERRNO_OS_ID (0x00U << 16)
#define ERRTYPE_ERROR (0x02U << 24)
#define ERRTYPE_FATAL (0x03U << 24)
#define ERRTYPE_NORMAL (0x00U << 24)
#define ERRTYPE_WARN (0x01U << 24)
/* From: src/include/uapi/prt_typedef.h */
#define FALSE ((bool)0)
/* From: src/include/uapi/prt_task.h */
#define GET_COREID(pid) ((U8)((pid) >> OS_TSK_TCB_INDEX_BITS))
#define GET_HANDLE(pid) ((pid) & ((1U << OS_TSK_TCB_INDEX_BITS) - 1))
/* From: src/utility/log/prt_log_internal.h */
#define HEAD_PTR_OFFSET 0U
/* From: src/include/uapi/prt_typedef.h */
#define INLINE static __inline __attribute__((always_inline))
#define LIKELY(x) __builtin_expect(!!(x), 1)
/* From: src/utility/log/prt_log_internal.h */
#define LOAD_FENCE() __asm__ __volatile__ ("mfence" : : : "memory");
#define LOG_MAX_SIZE (BUFFER_BLOCK_SIZE - sizeof(struct logHeader))
#define M_FENCE() __asm__ __volatile__ ("mfence" : : : "memory");
/* From: src/include/uapi/prt_typedef.h */
#define NO 0
#define NULL ((void *)0)
/* From: src/include/uapi/prt_sys.h */
#define OS_BOOT_PHASE 0x03
#define OS_BSSINIT_PHASE 0x06
#define OS_DEFAULT_PHASE 0x00
#define OS_DEVDRVINIT_PHASE 0x0b
/* From: src/include/uapi/prt_typedef.h */
#define OS_EMBED_ASM __asm__ __volatile__
/* From: src/include/uapi/prt_errno.h */
#define OS_ERRNO_BUILD_ERROR(mid, errno) (ERRTYPE_ERROR | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
#define OS_ERRNO_BUILD_FATAL(mid, errno) (ERRTYPE_FATAL | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
/* From: src/include/uapi/prt_task.h */
#define OS_ERRNO_BUILD_ID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x23)
/* From: src/include/uapi/prt_hook.h */
#define OS_ERRNO_HOOK_EXISTED OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x04)
#define OS_ERRNO_HOOK_FULL OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x05)
#define OS_ERRNO_HOOK_NOT_CFG OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x07)
#define OS_ERRNO_HOOK_NOT_EXISTED OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x06)
#define OS_ERRNO_HOOK_NO_MEMORY OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x02)
#define OS_ERRNO_HOOK_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x03)
#define OS_ERRNO_HOOK_TYPE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HOOK, 0x01)
/* From: src/include/uapi/prt_hwi.h */
#define OS_ERRNO_HWI_AFFINITY_HWINUM_SGI OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x18)
#define OS_ERRNO_HWI_AFFINITY_MASK_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x17)
#define OS_ERRNO_HWI_ALREADY_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x03)
#define OS_ERRNO_HWI_ATTR_CONFLICTED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x06)
#define OS_ERRNO_HWI_COMBINEHOOK_ALREADY_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x08)
#define OS_ERRNO_HWI_CORE_ID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0c)
#define OS_ERRNO_HWI_DELETED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0a)
#define OS_ERRNO_HWI_DELETE_INT OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x11)
#define OS_ERRNO_HWI_HW_REPORT_HWINO_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0d)
#define OS_ERRNO_HWI_MEMORY_ALLOC_FAILED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x07)
#define OS_ERRNO_HWI_MODE_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x09)
#define OS_ERRNO_HWI_MODE_UNSET OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0b)
#define OS_ERRNO_HWI_NUM_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x01)
#define OS_ERRNO_HWI_PRI_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x02)
#define OS_ERRNO_HWI_PROC_FUNC_NULL OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x04)
#define OS_ERRNO_HWI_RESOURCE_ALLOC_FAILED OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x0f)
#define OS_ERRNO_HWI_TRIGGER_HWINUM_NOT_SGI OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x14)
#define OS_ERRNO_HWI_TRIGGER_MASK_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x16)
#define OS_ERRNO_HWI_TRIGGER_TYPE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x15)
#define OS_ERRNO_HWI_UNCREATED OS_ERRNO_BUILD_FATAL(OS_MID_HWI, 0x05)
#define OS_ERRNO_MULTI_TARGET_CORE OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x10)
/* From: src/include/uapi/prt_atomic.h */
#define OS_ERRNO_SPL_ADDR_SIZE_ILLEGAL OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x00)
#define OS_ERRNO_SPL_ADDR_SIZE_NOT_ALIGN OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x02)
#define OS_ERRNO_SPL_ALLOC_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x04)
#define OS_ERRNO_SPL_FREE_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x06)
#define OS_ERRNO_SPL_INIT_FAIL OS_ERRNO_BUILD_FATAL(OS_MID_SPL, 0x07)
#define OS_ERRNO_SPL_NO_FREE OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x05)
#define OS_ERRNO_SPL_SIZE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x03)
/* From: src/include/uapi/prt_sys.h */
#define OS_ERRNO_SYS_CLOCK_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x02)
#define OS_ERRNO_SYS_CORE_RUNNUM_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x07)
#define OS_ERRNO_SYS_HWI_MAX_NUM_CONFIG_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x06)
#define OS_ERRNO_SYS_NO_CPUP_WARN OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x03)
#define OS_ERRNO_SYS_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x01)
#define OS_ERRNO_SYS_RND_PARA_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x04)
#define OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x05)
/* From: src/include/uapi/prt_task.h */
#define OS_ERRNO_TSK_ACTIVE_FAILED OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x14)
#define OS_ERRNO_TSK_ALREADY_SUSPENDED OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x09)
#define OS_ERRNO_TSK_BIND_CORE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x1d)
#define OS_ERRNO_TSK_BIND_IN_HWI OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x1e)
#define OS_ERRNO_TSK_BIND_SELF_WITH_TASKLOCK OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x1f)
#define OS_ERRNO_TSK_DELAY_IN_INT OS_ERRNO_BUILD_FATAL(OS_MID_TSK, 0x0d)
#define OS_ERRNO_TSK_DELAY_IN_LOCK OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x0e)
#define OS_ERRNO_TSK_DELETE_LOCKED OS_ERRNO_BUILD_FATAL(OS_MID_TSK, 0x0c)
#define OS_ERRNO_TSK_DESTCORE_NOT_RUNNING OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x22)
#define OS_ERRNO_TSK_ENTRY_NULL OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x05)
#define OS_ERRNO_TSK_EXIT_WITH_RESOURCE OS_ERRNO_BUILD_FATAL(OS_MID_TSK, 0x19)
#define OS_ERRNO_TSK_GET_CURRENT_COREID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x21)
#define OS_ERRNO_TSK_HAVE_MUTEX_SEM OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x28)
#define OS_ERRNO_TSK_ID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x08)
#define OS_ERRNO_TSK_INPUT_NUM_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x16)
#define OS_ERRNO_TSK_MAX_NUM_NOT_SUITED OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x15)
#define OS_ERRNO_TSK_NAME_EMPTY OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x06)
#define OS_ERRNO_TSK_NOT_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x0b)
#define OS_ERRNO_TSK_NOT_SUSPENDED OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x0a)
#define OS_ERRNO_TSK_NO_MEMORY OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x01)
#define OS_ERRNO_TSK_OPERATE_IDLE OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x12)
#define OS_ERRNO_TSK_OPERATION_BUSY OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x1c)
#define OS_ERRNO_TSK_PEND_MUTEX OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x25)
#define OS_ERRNO_TSK_PEND_PRIOR OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x26)
#define OS_ERRNO_TSK_PRIORITY_INHERIT OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x24)
#define OS_ERRNO_TSK_PRIOR_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x04)
#define OS_ERRNO_TSK_PRIOR_LOWER_THAN_PENDTSK OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x27)
#define OS_ERRNO_TSK_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x02)
#define OS_ERRNO_TSK_QUEUE_DOING OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x18)
#define OS_ERRNO_TSK_STACKADDR_NOT_ALIGN OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x17)
#define OS_ERRNO_TSK_STACKADDR_TOO_BIG OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x1b)
#define OS_ERRNO_TSK_STKSZ_NOT_ALIGN OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x03)
#define OS_ERRNO_TSK_STKSZ_TOO_SMALL OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x07)
#define OS_ERRNO_TSK_SUSPEND_LOCKED OS_ERRNO_BUILD_FATAL(OS_MID_TSK, 0x13)
#define OS_ERRNO_TSK_TCB_UNAVAILABLE OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x11)
#define OS_ERRNO_TSK_UNLOCK_NO_LOCK OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x1a)
#define OS_ERRNO_TSK_YIELD_INVALID_TASK OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x0f)
#define OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x10)
/* From: src/include/uapi/prt_typedef.h */
#define OS_ERROR (U32)(-1)
/* From: src/include/uapi/prt_hwi.h */
#define OS_ERROR_HWI_BASE_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x12)
#define OS_ERROR_HWI_INT_LOCK_REG_PARA_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x13)
#define OS_ERROR_HWI_INT_REGISTER_FAILED OS_ERRNO_BUILD_FATAL(OS_MID_HWI, 0x0e)
/* From: src/include/uapi/prt_typedef.h */
#define OS_FAIL 1
/* From: src/include/uapi/prt_hwi.h */
#define OS_HWI_ATTR(mode, type) (HwiMode)((HwiMode)(mode) | (HwiMode)(type))
/* From: src/include/uapi/prt_task.h */
#define OS_HWI_HANDLE ((1U << OS_TSK_TCB_INDEX_BITS) - 1)
/* From: src/include/uapi/prt_hwi.h */
#define OS_HWI_IPI_NO_00 0
#define OS_HWI_IPI_NO_01 1
#define OS_HWI_IPI_NO_011 11
#define OS_HWI_IPI_NO_012 12
#define OS_HWI_IPI_NO_013 13
#define OS_HWI_IPI_NO_014 14
#define OS_HWI_IPI_NO_015 15
#define OS_HWI_IPI_NO_02 2
#define OS_HWI_IPI_NO_03 3
#define OS_HWI_IPI_NO_04 4
#define OS_HWI_IPI_NO_05 5
#define OS_HWI_IPI_NO_06 6
#define OS_HWI_IPI_NO_07 7
#define OS_HWI_IPI_NO_08 8
#define OS_HWI_IPI_NO_09 9
#define OS_HWI_IPI_NO_10 10
#define OS_HWI_MODE_COMBINE 0x8000
#define OS_HWI_MODE_DEFAULT OS_HWI_MODE_ENGROSS
#define OS_HWI_MODE_ENGROSS 0x4000
#define OS_HWI_TYPE_NORMAL 0x00
/* From: src/include/uapi/prt_sys.h */
#define OS_INITIALIZE_PHASE 0x0a
/* From: src/include/uapi/prt_typedef.h */
#define OS_INVALID (-1)
/* From: src/include/uapi/prt_atomic.h */
#define OS_INVALID_LOCK_OWNER_ID 0xFFFFFFFF
/* From: src/include/uapi/prt_sys.h */
#define OS_LIBCINIT_PHASE 0x08
/* From: src/include/uapi/prt_atomic.h */
#define OS_LOCK_OWNER_SYS 0xFFFFFFFF
/* From: src/include/uapi/prt_typedef.h */
#define OS_OK 0
/* From: src/include/uapi/prt_sys.h */
#define OS_REGISTER_PHASE 0x09
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
/* From: src/include/uapi/prt_atomic.h */
#define OS_SPINLOCK_LOCK 1
#define OS_SPINLOCK_UNLOCK 0
/* From: src/include/uapi/prt_sys.h */
#define OS_START_PHASE 0x0c
#define OS_SYS_APP_VER_LEN 64
#define OS_SYS_MS_PER_SECOND 1000
#define OS_SYS_NS_PER_SECOND 1000000000
#define OS_SYS_OS_VER_LEN 48
#define OS_SYS_US_PER_SECOND 1000000
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_CRG_IDLE_SUSPEND 0X20000U
#define OS_TSK_DELAY 0x0020
#define OS_TSK_DELAY_INTERRUPTIBLE 0x8000
#define OS_TSK_DELETING 0X8000U
#define OS_TSK_EVENT_PEND 0x0800
#define OS_TSK_EVENT_TYPE 0x1000
#define OS_TSK_HOLD_SIGNAL 0x0400
#define OS_TSK_INUSE 0x0001
#define OS_TSK_NULL_ID 0xFFFFFFFF
#define OS_TSK_PEND 0x0008
#define OS_TSK_PRIORITY_00 0
#define OS_TSK_PRIORITY_01 1
#define OS_TSK_PRIORITY_02 2
#define OS_TSK_PRIORITY_03 3
#define OS_TSK_PRIORITY_04 4
#define OS_TSK_PRIORITY_05 5
#define OS_TSK_PRIORITY_06 6
#define OS_TSK_PRIORITY_07 7
#define OS_TSK_PRIORITY_08 8
#define OS_TSK_PRIORITY_09 9
#define OS_TSK_PRIORITY_10 10
#define OS_TSK_PRIORITY_11 11
#define OS_TSK_PRIORITY_12 12
#define OS_TSK_PRIORITY_13 13
#define OS_TSK_PRIORITY_14 14
#define OS_TSK_PRIORITY_15 15
#define OS_TSK_PRIORITY_16 16
#define OS_TSK_PRIORITY_17 17
#define OS_TSK_PRIORITY_18 18
#define OS_TSK_PRIORITY_19 19
#define OS_TSK_PRIORITY_20 20
#define OS_TSK_PRIORITY_21 21
#define OS_TSK_PRIORITY_22 22
#define OS_TSK_PRIORITY_23 23
#define OS_TSK_PRIORITY_24 24
#define OS_TSK_PRIORITY_25 25
#define OS_TSK_PRIORITY_26 26
#define OS_TSK_PRIORITY_27 27
#define OS_TSK_PRIORITY_28 28
#define OS_TSK_PRIORITY_29 29
#define OS_TSK_PRIORITY_30 30
#define OS_TSK_PRIORITY_31 31
#define OS_TSK_PRIORITY_32 32
#define OS_TSK_PRIORITY_33 33
#define OS_TSK_PRIORITY_34 34
#define OS_TSK_PRIORITY_35 35
#define OS_TSK_PRIORITY_36 36
#define OS_TSK_PRIORITY_37 37
#define OS_TSK_PRIORITY_38 38
#define OS_TSK_PRIORITY_39 39
#define OS_TSK_PRIORITY_40 40
#define OS_TSK_PRIORITY_41 41
#define OS_TSK_PRIORITY_42 42
#define OS_TSK_PRIORITY_43 43
#define OS_TSK_PRIORITY_44 44
#define OS_TSK_PRIORITY_45 45
#define OS_TSK_PRIORITY_46 46
#define OS_TSK_PRIORITY_47 47
#define OS_TSK_PRIORITY_48 48
#define OS_TSK_PRIORITY_49 49
#define OS_TSK_PRIORITY_50 50
#define OS_TSK_PRIORITY_51 51
#define OS_TSK_PRIORITY_52 52
#define OS_TSK_PRIORITY_53 53
#define OS_TSK_PRIORITY_54 54
#define OS_TSK_PRIORITY_55 55
#define OS_TSK_PRIORITY_56 56
#define OS_TSK_PRIORITY_57 57
#define OS_TSK_PRIORITY_58 58
#define OS_TSK_PRIORITY_59 59
#define OS_TSK_PRIORITY_60 60
#define OS_TSK_PRIORITY_61 61
#define OS_TSK_PRIORITY_62 62
#define OS_TSK_PRIORITY_63 63
#define OS_TSK_QUEUE_BUSY 0x4000
#define OS_TSK_QUEUE_PEND 0x2000
#define OS_TSK_READY 0x0040
#define OS_TSK_RUNNING 0x0080
#define OS_TSK_RW_PEND 0x40000
#define OS_TSK_SCHED_FIFO 0x0U
#define OS_TSK_SCHED_RR 0x1U
#define OS_TSK_SET_PERIODIC 0x80000
#define OS_TSK_SIG_PAUSE 0x20000
#define OS_TSK_SUSPEND 0x0004
#define OS_TSK_TCB_INDEX_BITS ((4 - OS_TSK_CORE_BYTES_IN_PID) * 8)
#define OS_TSK_TIMEOUT 0x0010
#define OS_TSK_UNUSED 0x0000
#define OS_TSK_WAITQUEUE_PEND 0x10000
#define OS_TSK_WAIT_SIGNAL 0x0002
/* From: src/include/uapi/prt_atomic.h */
#define PRT_ATOMIC_H
/* From: src/arch/include/prt_attr_external.h */
#define PRT_ATTR_EXTERNAL_H
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/include/uapi/prt_hook.h */
#define PRT_HOOK_H
/* From: src/include/uapi/prt_hwi.h */
#define PRT_HWI_H
/* From: src/include/uapi/prt_log.h */
#define PRT_IsLogInit() true
#define PRT_LOG_H
/* From: src/utility/log/prt_log_internal.h */
#define PRT_LOG_INTERNAL_H
/* From: src/include/uapi/prt_log.h */
#define PRT_Log(level, facility, str, strLen) \
    (((level) >= OS_LOG_NONE || (str) == NULL) ? ((U32)(-1)) : 0)
/* PRT_LogFormat is defined in func_under_test.c */
/* PRT_LogInit is defined in func_under_test.c */
#define PRT_LogOff() 
#define PRT_LogOn() 
#define PRT_LogSetFilter(level) 0
#define PRT_LogSetFilterByFacility(facility, level) 0
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/include/uapi/prt_sys.h */
#define PRT_SYS_H
/* From: src/include/uapi/prt_task.h */
#define PRT_TASK_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
/* From: platform/libboundscheck/include/securec.h */
#define SECUREC_H
/* From: src/include/uapi/prt_log.h */
#define SHM_MAP_SIZE 0x1100000UL
/* From: src/utility/log/prt_log_internal.h */
#define SHM_USED_SIZE 0x1004200UL // ~16MB
#define STORE_FENCE() __asm__ __volatile__ ("mfence" : : : "memory");
#define TAIL_PTR_OFFSET sizeof(void*)
/* From: src/include/uapi/prt_task.h */
#define TM_INFINITE 0
#define TM_NOW 0
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
/* From: src/utility/log/prt_log_internal.h */
#define VALID_FLAGS_OFFSET 0U
#define VALID_FLAGS_SIZE 0x4000UL
/* From: src/include/uapi/prt_typedef.h */
#define YES 1
/* ---- Common runtime shims ---- */
/* Secure memset (used by QSemOS kernel code) */
/* PRT_LogInit now from source */

/* Additional log stub functions */
static struct PrtSpinLock g_logLock;
static volatile U32 g_sequenceNum = 0;
static uintptr_t g_logMemBase = 0;
static int g_logOn = 1;
static U32 g_logFilter[8] = {0};

static U32 PRT_SplLockInit(struct PrtSpinLock *lock) { (void)lock; return 0; }

static uintptr_t OsLogLockOn(void) { return 0; }
static void OsLogLockRestore(uintptr_t value) { (void)value; }

static void OsLogGetTailAndHead(U32 *head, U32 *tail)
{
    volatile U32 *const headPtr = (U32 *)(g_logMemBase + HEAD_PTR_OFFSET);
    volatile U32 *const tailPtr = (U32 *)(g_logMemBase + TAIL_PTR_OFFSET);
    *head = *headPtr;
    *tail = *tailPtr;
}

static U32 OsLogUpdateTail(U32 *tail)
{
    volatile U32 *const tailPtr = (U32 *)(g_logMemBase + TAIL_PTR_OFFSET);
    U32 currHead, currTail;

    OsLogGetTailAndHead(&currHead, &currTail);

    if (currTail < currHead) {
        return -1;
    }

    if (currTail >= currHead + BUFFER_BLOCK_NUM) {
        return -1;
    }

    currTail += 1;
    *tailPtr = currTail;
    *tail = currTail;
    return 0;
}

static U32 OsCheckLog(enum OsLogLevel level, enum OsLogFacility facility) {
    if (level >= OS_LOG_NONE || facility < OS_LOG_F0 || facility > OS_LOG_F7) {
        return 1;
    }
    return 0;
}

static U32 OsLog(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen)
{
    uintptr_t intSave;
    U32 currTail, logIndex;
    U8 *targetMem;
    struct logHeader header;
    volatile U8 *const validPtr = (U8 *)(g_logMemBase + VALID_FLAGS_OFFSET);

    if (g_logMemBase == 0) {
        return -1;
    }

    intSave = OsLogLockOn();
    currTail = g_sequenceNum;
    g_sequenceNum++;
    OsLogLockRestore(intSave);

    if (OsCheckLog(level, facility) || str == NULL || strLen == 0) {
        return -1;
    }

    if ((!g_logOn) || (level >= g_logFilter[facility - OS_LOG_F0])) {
        return 0;
    }

    if (strLen > LOG_MAX_SIZE) {
        return -1;
    }

    OsLogUpdateTail(&currTail);

    logIndex = currTail % (2 * BUFFER_BLOCK_NUM);
    validPtr[logIndex] = 1;

    return 0;
}

extern volatile int g_hwi_lock_count;
#endif /* AUTO_STUB_PRT_LOG_H */
