/*
 * Auto-generated stub header for prt_sys_time.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/core/kernel/sys/prt_sys_time.c
 * Included headers: 11
 */
#ifndef AUTO_STUB_PRT_SYS_TIME_H
#define AUTO_STUB_PRT_SYS_TIME_H
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

/* Forward declarations */
struct TagTskCb;
struct TagOsRunQue;
struct TagScheduleClass;
struct TimerApi;

/* Missing types for POSIX/signal support */
typedef U32 signalSet;
typedef void *timer_t;
typedef void *locale_t;
typedef void *_sa_handler;
struct filelist { void *dummy; };
struct streamlist { void *dummy; };
struct task_struct { void *dummy; };
struct __ptcb { void *dummy; };

/* ---- Structural macros (used in type definitions) ---- */
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_CORE_MASK U32
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_CORE_STR_MAX_LEN 4
/* From: src/core/kernel/include/prt_plist_external.h */
#define OS_GET_WORD_NUM_BY_PRIONUM(prio) (((prio) + 0x1f) >> 5) //通过支持的优先级个数计算需要多少个word表示
/* From: src/include/uapi/prt_buildef.h */
#define OS_OPTION_EVENT
#define OS_OPTION_PERF
#define OS_OPTION_SMP
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_NAME_LEN 16
/* From: src/include/uapi/prt_buildef.h */
#define OS_TSK_NUM_OF_PRIORITIES 32
/* From: src/include/uapi/prt_signal.h */
#define PRT_SIGNAL_MAX 32
/* From: src/core/kernel/include/prt_task_external.h */
#define PTHREAD_KEYS_MAX 32
/* ---- Types ---- */
/* From: src/core/kernel/include/prt_sys_external.h */
typedef void (*TickDispFunc)(void);
typedef void (*TickEntryFunc)(void);
typedef void (*TaskScanFunc)(void);
/* From: src/include/uapi/prt_sys.h */
typedef void (*PrtIdleHook)(void);
/* From: src/include/uapi/prt_task.h */
typedef void (*TskEntryFunc)(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
/* From: src/include/uapi/prt_tick.h */
typedef void (*TickHandleFunc)(void);
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
/* From: src/core/kernel/include/prt_sys_external.h */
struct CoreNumStr {
    char coreNo[OS_CORE_STR_MAX_LEN];
};
/* From: src/libc/litelibc/include/bits/list_types.h */
struct TagListObject {
    struct TagListObject *prev;
    struct TagListObject *next;
};
/* From:  */
enum SysThreadType {
    SYS_HWI,      /* 当前线程类型属于硬中断 */
    SYS_TASK,     /* 当前线程类型属于任务 */
    SYS_KERNEL,   /* 当前线程类型属于内核 */
    SYS_BUTT
};
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
enum SysRndNumType {
    OS_SYS_RND_STACK_PROTECT,     /* 栈保护的RND值 */
    OS_SYS_RND_BUTT               /* 非法类型 */
};
enum SleepMode {
    OS_TYPE_LIGHT_SLEEP = 0, /* 浅睡唤醒补偿方式 */
    OS_TYPE_DEEP_SLEEP, /* 深睡唤醒补偿方式 */
    OS_TYPE_INVALID_SLEEP, /* 无效补偿方式 */
};
/* From: src/include/uapi/prt_task.h */
typedef U16 TskStatus;
/* From: src/include/uapi/prt_sem.h */
typedef U16 SemHandle;
/* From: src/include/uapi/prt_task.h */
typedef U16 TskPrior;
/* From: src/include/uapi/prt_tick.h */
struct TickModInfo {
    /* Tick中断的优先级配置 */
    U32 tickPriority;
    /* 每秒产生的TICK中断数 */
    U32 tickPerSecond;
};
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
/* From: src/include/uapi/prt_sys.h */
typedef U32 (*CoreWakeUpHook)(U32 coreId);
/* From: src/include/uapi/prt_hook.h */
struct HookModInfo {
    U8 maxNum[(U32)OS_HOOK_TYPE_NUM];
};
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
/* From: src/core/kernel/include/prt_rt_external.h */
struct RtActiveTskList
{
    /* 优先级bit位表 */
    U32 readyPrioBit[OS_GET_WORD_NUM_BY_PRIONUM(OS_TSK_NUM_OF_PRIORITIES)];
    struct TagListObject readyList[OS_TSK_NUM_OF_PRIORITIES];
};
/* From: src/core/kernel/include/prt_plist_external.h */
struct PushablTskListHead
{
    struct TagListObject nodeList;
};
struct PushablTskList
{
    U32 prio;
    struct TagListObject nodeList;
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
    U32 systemClock;
    SysTimeFunc sysTimeHook;
    U32 cpuType;
#if defined(OS_OPTION_SMP)
    U8 coreRunNum;
    U8 coreMaxNum;
    U8 corePrimary;
#if defined(OS_OPTION_HWI_MAX_NUM_CONFIG)
    U32 hwiMaxNum;
#endif
#endif
};
/* From: src/core/kernel/include/prt_rt_external.h */
struct RtRq {
    /* 该队列是否overload*/
    bool isOverload;
    /* pushable链表中最高的优先级 */
    U32 nextPrio;
    /* 运行队列上任务的个数 */
    U32 nrRunning;
    /* ready链表 */
    struct RtActiveTskList activeTsk;
    /* 可push链表 */
    struct PushablTskListHead pushAblList;
};
/* From: src/core/kernel/include/prt_sched_external.h */
struct TagOsRunQue {
    volatile uintptr_t spinLock;        // 操作该RQ使用的锁
    U32 uniFlag;
    struct TagTskCb *tskCurr;           // RQ中的当前运行任务
    bool needReschedule;                // 是否需要调度
    U32 rqCoreId;                       // 运行队列所属的核
    U32 tskIdlePID;                     // RQ中的idle任务
    U32 nrRunning;                      // RQ中总运行任务个数
    U32 intCount;                       // 中断进入次数
    U32 tickNoRespondCnt;               // tick待响应次数
    U16 uniTaskLock;                    // 锁任务计数
    U16 reserved;
    U32 shakeCount;                     // 核间握手计数
    bool online;                        // 队列是否还在线
    U32 currntPrio;                     // RQ中最高优先级任务的优先级
    struct RtRq rtRq;                   // 实时优先级运行队列
    struct TagScheduleClass *schedClass; //调度方法
};
/* From: src/core/kernel/include/prt_task_external.h */
struct TagScheduleClass {
    void (*osEnqueueTask)(struct TagOsRunQue *runQue, struct TagTskCb *tsk, U32 flags);
    void (*osDequeueTask)(struct TagOsRunQue *runQue, struct TagTskCb *tsk, U32 flags);
    void (*osPutPrevTask)(struct TagOsRunQue *runQue, struct TagTskCb *prevTsk);
    struct TagTskCb *(*osPickNextTask)(struct TagOsRunQue *runQue);
    struct TagTskCb *(*osNextReadyTask)(struct TagOsRunQue *runQue);
};
struct TagTskCb {
    void *stackPointer;
    U32 taskStatus;
    U32 stackSize;
    TskHandle taskPid;
    uintptr_t topOfStack;
    TskEntryFunc taskEntry;
    void *taskPend;
    uintptr_t args[4];
    char name[OS_TSK_NAME_LEN];
    TskPrior priority;
    TskPrior origPriority;
    U16 policy;
    U32 timeSlice;
    U32 lastErr;
    U32 smpReserve;
    struct PushablTskList pushAbleList;
    U32 coreID;
    bool isOnRq;
    struct TagScheduleClass *scheClass;
    U64 expirationTick;
    U8 state;
    U8 cancelState;
    U8 cancelType;
    U8 cancelPending;
    void *retval;
    U16 joinCount;
    SemHandle joinableSem;
    void *tsd[PTHREAD_KEYS_MAX];
    U32 tsdUsed;
    U32 event;
    U32 eventMask;
    struct TagListObject pendList;
    struct TagListObject timerList;
    struct TagListObject semBList;
    struct TagListObject waitList;
    struct filelist tskFileList;
    void *stdio_locks;
    struct streamlist ta_streamlist;
    uintptr_t pc;
    uintptr_t fp;
};
/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))
/* From: src/core/kernel/include/prt_sys_external.h */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))
/* From: src/arch/include/prt_cpu_external.h */
#define ASM_NOP() 
/* From: src/core/kernel/include/prt_list_external.h */
#define COMPLEX_OF(ptr, type, field) ((type *)((uintptr_t)(ptr) - OFFSET_OF_FIELD(type, field)))
/* From: src/include/uapi/prt_task.h */
#define COMPOSE_PID(coreid, handle) 
/* From: src/arch/cpu/armv7-m/common/os_cpu_armv7_m_external.h */
#define DIV64(a, b) OsU64DivGetQuotient((a), (b))
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
/* From: src/core/kernel/include/prt_list_external.h */
#define INIT_LIST_OBJECT(object) 
/* From: src/include/uapi/prt_typedef.h */
#define INLINE static __inline __attribute__((always_inline))
#define LIKELY(x) __builtin_expect(!!(x), 1)
/* From: src/core/kernel/include/prt_list_external.h */
#define LIST_COMPONENT(ptrOfList, typeOfList, fieldOfList) COMPLEX_OF(ptrOfList, typeOfList, fieldOfList)
#define LIST_FIRST_ENTITY(listForFirstEntity, typeOfList, field) 
#define LIST_FOR_EACH(posOfList, listObject, typeOfList, field) 
#define LIST_FOR_EACH_SAFE(posOfList, listObject, typeOfList, field) 
#define LIST_LAST(object) ((object)->prev)
#define LIST_OBJECT_INIT(object) {
/* From: src/include/uapi/prt_typedef.h */
#define NO 0
/* From: src/arch/include/prt_cpu_external.h */
#define NOP1() 
#define NOP4() 
#define NOP8() 
/* From: src/include/uapi/prt_typedef.h */
#define NULL ((void *)0)
/* From: src/core/kernel/include/prt_list_external.h */
#define OFFSET_OF_FIELD(type, field) ((uintptr_t)((uintptr_t)(&((type *)0x10)->field) - (uintptr_t)0x10))
/* From: src/include/uapi/prt_sys.h */
#define OS_BOOT_PHASE 0x03
#define OS_BSSINIT_PHASE 0x06
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_CORE_STR_END_INDEX (OS_CORE_STR_MAX_LEN - 1)   // 结束字符位置索引
#define OS_CORE_STR_NUM_INDEX (OS_CORE_STR_END_INDEX -1)  // 最后一个有效字符位置索引
/* From: src/include/uapi/prt_sys.h */
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
/* From: src/include/uapi/prt_sys.h */
#define OS_ERRNO_SYS_CLOCK_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x02)
#define OS_ERRNO_SYS_CORE_RUNNUM_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x07)
#define OS_ERRNO_SYS_HWI_MAX_NUM_CONFIG_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x06)
#define OS_ERRNO_SYS_NO_CPUP_WARN OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x03)
#define OS_ERRNO_SYS_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x01)
#define OS_ERRNO_SYS_RND_PARA_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x04)
#define OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED OS_ERRNO_BUILD_ERROR(OS_MID_SYS, 0x05)
/* From: src/include/uapi/prt_tick.h */
#define OS_ERRNO_TICK_ADJUST_MODE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TICK, 0x04)
#define OS_ERRNO_TICK_ADJUST_VALUE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TICK, 0x03)
#define OS_ERRNO_TICK_PERIOD OS_ERRNO_BUILD_ERROR(OS_MID_TICK, 0x01)
#define OS_ERRNO_TICK_PER_SECOND_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_TICK, 0x02)
#define OS_ERRNO_TICK_REG_HOOK_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_TICK, 0x05)
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
#define OS_FAIL 1
/* From: src/arch/include/prt_asm_cpu_external.h */
#define OS_FLG_BGD_ACTIVE 0x0002
#define OS_FLG_EXC_ACTIVE 0x0020
#define OS_FLG_HWI_ACTIVE 0x0001
#define OS_FLG_SYS_ACTIVE 0x0010
#define OS_FLG_TICK_ACTIVE 0x0008
/* From: src/arch/include/prt_cpu_external.h */
#define OS_GOTO_SYS_ERROR() 
#define OS_GOTO_SYS_ERROR1() OS_GOTO_SYS_ERROR()
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_HWI_ACTIVE ((UNI_FLAG & OS_HWI_ACTIVE_MASK) != 0)
#define OS_HWI_ACTIVE_MASK (OS_FLG_HWI_ACTIVE | OS_FLG_TICK_ACTIVE | OS_FLG_SYS_ACTIVE | OS_FLG_EXC_ACTIVE)
/* From: src/include/uapi/prt_task.h */
#define OS_HWI_HANDLE ((1U << OS_TSK_TCB_INDEX_BITS) - 1)
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_HWI_INACTIVE (!(OS_INT_ACTIVE))
/* From: src/include/uapi/prt_sys.h */
#define OS_INITIALIZE_PHASE 0x0a
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_INT_ACTIVE ((UNI_FLAG & OS_INT_ACTIVE_MASK) != 0)
#define OS_INT_ACTIVE_MASK
#define OS_INT_INACTIVE (!(OS_INT_ACTIVE))
/* From: src/include/uapi/prt_typedef.h */
#define OS_INVALID (-1)
/* From: src/include/uapi/prt_sys.h */
#define OS_LIBCINIT_PHASE 0x08
/* From: src/core/kernel/include/prt_list_external.h */
#define OS_LIST_FIRST(object) ((object)->next)
#define OS_LIST_INIT(head) 
/* From: src/include/uapi/prt_buildef.h */
#define OS_MAX_CORE_NUM 4
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_MS2CYCLE(ms, clock) DIV64(((ms) * (U64)(clock)), OS_SYS_MS_PER_SECOND) /* 毫秒转换成cycle */
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
/* From: src/include/uapi/prt_sys.h */
#define OS_START_PHASE 0x0c
#define OS_SYS_APP_VER_LEN 64
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_SYS_HWI_STATUS(flag) (((flag) & OS_FLG_HWI_ACTIVE) != 0)
/* From: src/include/uapi/prt_sys.h */
#define OS_SYS_MS_PER_SECOND 1000
#define OS_SYS_NS_PER_SECOND 1000000000
#define OS_SYS_OS_VER_LEN 48
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_SYS_PID_BASE (0x0U << OS_TSK_TCB_INDEX_BITS)
#define OS_SYS_STACK_TOP_MAGIC 0xC5A2C1C6
#define OS_SYS_TASK_STATUS(flag) (((flag) & OS_THREAD_FLAG_MASK) == OS_FLG_BGD_ACTIVE)
/* From: src/include/uapi/prt_sys.h */
#define OS_SYS_US_PER_SECOND 1000000
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_TASK_SCAN() 
#define OS_THREAD_FLAG_MASK
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
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_US2CYCLE(us, clock) DIV64(((us) * (U64)(clock)), OS_SYS_US_PER_SECOND) /* 微秒转换成cycle */
#define OS_VAR_ARRAY_NUM OS_MAX_CORE_NUM
/* From: src/arch/include/prt_attr_external.h */
#define PRT_ATTR_EXTERNAL_H
/* From: src/arch/include/prt_cpu_external.h */
#define PRT_CPU_EXTERNAL_H
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/include/uapi/prt_hook.h */
#define PRT_HOOK_H
/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_EXTERNAL_H
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/core/kernel/include/prt_sys_external.h */
#define PRT_SYS_EXTERNAL_H
/* From: src/include/uapi/prt_sys.h */
#define PRT_SYS_H
/* From: src/include/uapi/prt_task.h */
#define PRT_TASK_H
/* From: src/include/uapi/prt_tick.h */
#define PRT_TICK_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
/* From: src/core/kernel/include/prt_sys_external.h */
#define THIS_CORE() OS_THIS_CORE
/* From: src/core/kernel/include/prt_sched_external.h */
#define THIS_RUNQ() ((struct TagOsRunQue *)&g_runQueue[THIS_CORE()])
/* From: src/core/kernel/include/prt_sys_external.h */
#define TICK_NO_RESPOND_CNT g_tickNoRespondCnt
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
/* From: src/core/kernel/include/prt_sys_external.h */
#define UNI_FLAG (THIS_RUNQ()->uniFlag)
/* From: src/include/uapi/prt_typedef.h */
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
struct TagSemCb;
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
/* 64-bit division functions */
static inline U64 OsU64DivGetQuotient(U64 dividend, U64 divisor)
{
    return dividend / divisor;
}
static inline U64 OsU64DivGetRemainder(U64 dividend, U64 divisor)
{
    return dividend % divisor;
}
/* ---- Common globals (stubs for kernel-BSS variables) ---- */
static U32 g_tskBaseId = 0;
static U32 g_tskMaxNum = 0;
static U32 g_idleTaskId = 0;
static U32 g_maxNumOfCores = 1;
static U32 g_uniTaskLock = 0;
static U32 g_tickNoRespondCnt = 0;
static U32 g_createTskLock = 0;
static struct TagTskCb *g_tskCbArray = NULL;
static struct TagOsRunQue g_runQueue[1];
static char g_tskSortedDelay[128];  /* placeholder */
static struct TagTskCb *g_runningTask = NULL;
static void *g_semTsk = NULL;
static void *g_tskSem = NULL;
static struct TagScheduleClass g_osRtSingleSchedClass = {0};
typedef U64 (*SysTimeHook)(void);
SysTimeHook g_sysTimeHook = NULL;
#endif /* AUTO_STUB_PRT_SYS_TIME_H */
