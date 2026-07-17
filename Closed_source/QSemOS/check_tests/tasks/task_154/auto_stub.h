/*
 * Auto-generated stub header for prt_sched_single.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/core/kernel/sched/prt_sched_single.c
 * Included headers: 30
 */
#ifndef AUTO_STUB_PRT_SCHED_SINGLE_H
#define AUTO_STUB_PRT_SCHED_SINGLE_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef void *VirtAddr;
typedef void *PhyAddr;

/* Forward declarations */
struct TagTskCb;
struct TagOsRunQue;
struct TagScheduleClass;
struct TimerApi;
struct TagSwTmrCtrl;

/* Missing types for POSIX/signal support */
typedef void *timer_t;
typedef void *locale_t;
struct filelist { void *dummy; };
struct streamlist { void *dummy; };
struct task_struct { void *dummy; };
struct __ptcb { void *dummy; };

/* Additional missing definitions */
#define OS_THIS_CORE 0
#define OS_FLG_TSK_REQ 0x0004

/* External globals from test_support.c */
extern struct TagTskCb *RUNNING_TASK;
extern struct TagTskCb *g_highestTask;
extern struct TagTskCb g_taskA;
extern struct TagTskCb g_taskB;
extern volatile uint32_t UNI_FLAG;
extern volatile uint32_t g_uniTaskLock;
extern volatile uint32_t TICK_NO_RESPOND_CNT;
extern void (*g_tickDispatcher)(void);
extern U64 g_fakeTime;
extern uintptr_t g_lastContextLoadPtr;
extern uint32_t g_contextLoadCount;
extern uint32_t g_switchHookCount;
extern uint32_t g_lastPrevPid;
extern uint32_t g_lastNextPid;
extern uint32_t g_irqRecordCount;
extern uint32_t g_intEnableCount;
extern uint32_t g_intDisableCount;
extern void test_tick_dispatcher_noop(void);
extern void reset_test_state(void);
extern void OsTskReadyDel(struct TagTskCb *tsk);
extern void OsTskReadyAdd(struct TagTskCb *tsk);
extern void OsTskContextLoad(uintptr_t tcbPtr);
extern void OsTskHighestSet(void);
extern void OsTimeSliceUpdate(struct TagTskCb *tsk, U64 curr, uint32_t discount);
extern void OsTskSwitchHookCaller(uint32_t prevPid, uint32_t nextPid);
extern U64 OsCurCycleGet64(void);
extern void OsIntEnable(void);
extern void OsIntDisable(void);
extern void OS_IRQ_TIME_RECORD(U64 start);

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
/* From: src/core/kernel/include/prt_task_external.h */
typedef void (*TskCoresleep)(void);
typedef void (*PowerOffFuncT)(void);
typedef void (*SetOfflineFlagFuncT)(void);
/* From: src/core/kernel/include/prt_tick_external.h */
typedef void(*SwitchScanFunc)(void);
typedef void (*TskmonTickHook)(void);
/* From: src/include/uapi/prt_hwi.h */
typedef uintptr_t HwiArg;
/* From: src/include/uapi/prt_signal.h */
typedef unsigned long signalSet;
typedef void (*_sa_handler)(int);
typedef struct {
    int si_signo;
    int si_code;
} signalInfo;
/* From: src/include/uapi/prt_sys.h */
typedef void (*PrtIdleHook)(void);
/* From: src/include/uapi/prt_task.h */
typedef void (*TskEntryFunc)(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
/* From: src/include/uapi/prt_tick.h */
typedef void (*TickHandleFunc)(void);
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
/* From: src/om/include/prt_hook_external.h */
typedef void (*OsFunPara0)(void);
typedef void (*OsFunPara1)(uintptr_t);
typedef void (*OsFunPara2)(uintptr_t, uintptr_t);
typedef void (*OsFunPara3)(uintptr_t, uintptr_t, uintptr_t);
typedef void (*OsFunPara4)(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
typedef void (*OsFunPara5)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
/* From: src/core/kernel/include/prt_sys_external.h */
struct CoreNumStr {
    char coreNo[OS_CORE_STR_MAX_LEN];
};

/* From: src/include/uapi/prt_atomic.h */
/* struct OsSpinLock - not needed, using PrtSpinLock */
#ifndef STRUCT_PRT_SPINLOCK_DEFINED
#define STRUCT_PRT_SPINLOCK_DEFINED
#endif
typedef volatile U32 SplLock;
typedef struct PrtSpinLock {
    SplLock lock;
    uintptr_t rawLock;
} PrtSpinLock;

/* From: src/include/uapi/prt_signal.h */
struct _sigaction {
    _sa_handler saHandler;
};

/* From: src/mem/include/prt_mem_external.h */
struct TagFscMemCtrl {
    struct TagFscMemCtrl *next;
    // 块大小
    uintptr_t size;
    // 若前面相邻的物理块空闲，则此字段记录前面空闲块大小，否则为OS_FSC_MEM_PREV_USED
    uintptr_t prevSize;
    // 空闲时为上一个控制块地址
    struct TagFscMemCtrl *prev;
};

/* From: src/libc/litelibc/include/bits/list_types.h */
#ifndef STRUCT_TAGLISTOBJECT_DEFINED
#define STRUCT_TAGLISTOBJECT_DEFINED
struct TagListObject {
    struct TagListObject *prev;
    struct TagListObject *next;
};
#endif

/* From:  */
enum OsScheduleType{
    OS_SCHEDULE_RT_SINGLE //单硬线程调度
};

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

enum OsHwiIpiType {
    OS_TYPE_TRIGGER_BY_MASK = 0, /* 通过mask确定需要触发的目标核 */
    OS_TYPE_TRIGGER_TO_OTHER, /* 触发除本核外的其他核 */
    OS_TYPE_TRIGGER_TO_SELF, /* 触发本核 */
    OS_TYPE_TRIGGER_BUTT /* 非法 */
};

enum MemArith {
    MEM_ARITH_FSC,          // 私有FSC算法
    MEM_ARITH_BUTT          // 内存算法非法
};

enum MemAlign {
    MEM_ADDR_ALIGN_004 = 2, /* 4字节对齐 */
    MEM_ADDR_ALIGN_008 = 3, /* 8字节对齐 */
    MEM_ADDR_ALIGN_016 = 4, /* 16字节对齐 */
    MEM_ADDR_ALIGN_032 = 5, /* 32字节对齐 */
    MEM_ADDR_ALIGN_064 = 6, /* 64字节对齐 */
    MEM_ADDR_ALIGN_128 = 7, /* 128字节对齐 */
    MEM_ADDR_ALIGN_256 = 8, /* 256字节对齐 */
    MEM_ADDR_ALIGN_512 = 9, /* 512字节对齐 */
    MEM_ADDR_ALIGN_1K = 10, /* 1K字节对齐 */
    MEM_ADDR_ALIGN_2K = 11, /* 2K字节对齐 */
    MEM_ADDR_ALIGN_4K = 12, /* 4K字节对齐 */
    MEM_ADDR_ALIGN_8K = 13, /* 8K字节对齐 */
    MEM_ADDR_ALIGN_16K = 14, /* 16K字节对齐 */
    MEM_ADDR_BUTT /* 字节对齐非法 */
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

#if !defined(SEM_MODE_FIFO) && !defined(SEM_MODE_PRIOR)
enum SemMode {
    SEM_MODE_FIFO,  // 信号量FIFO唤醒模式
    SEM_MODE_PRIOR, // 信号量优先级唤醒模式
    SEM_MODE_BUTT   // 信号量非法唤醒方式
};
#endif

enum SysRndNumType {
    OS_SYS_RND_STACK_PROTECT,     /* 栈保护的RND值 */
    OS_SYS_RND_BUTT               /* 非法类型 */
};

enum SleepMode {
    OS_TYPE_LIGHT_SLEEP = 0, /* 浅睡唤醒补偿方式 */
    OS_TYPE_DEEP_SLEEP, /* 深睡唤醒补偿方式 */
    OS_TYPE_INVALID_SLEEP, /* 无效补偿方式 */
};

enum HookChgType {
    HOOK_ADD_FIRST,
    HOOK_DEL_LAST
};

/* From: src/include/uapi/prt_hwi.h */
typedef void (*HwiProcFunc)(HwiArg);
/* From: src/include/uapi/prt_task.h */
typedef U32 TskStatus;
/* From: src/include/uapi/prt_hwi.h */
typedef U16 HwiPrior;
/* From: src/include/uapi/prt_sem.h */
struct SemModInfo {
    /* 最大支持的信号量数 */
    U16 maxNum;
    /* 保留 */
    U16 reserved;
};

typedef uint32_t SemHandle;
/* From: src/include/uapi/prt_hwi.h */
typedef U16 HwiMode;
/* From: src/include/uapi/prt_task.h */
typedef uint32_t TskPrior;
/* From: src/include/uapi/prt_tick.h */
struct TickModInfo {
    /* Tick中断的优先级配置 */
    U32 tickPriority;
    /* 每秒产生的TICK中断数 */
    U32 tickPerSecond;
};

/* From: src/include/uapi/prt_hwi.h */
typedef U32 HwiHandle;
/* From: src/include/uapi/prt_task.h */
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

/* From: src/include/uapi/prt_hwi.h */
typedef void (*HwiEntryHook)(U32 hwiNum);
/* From: src/core/kernel/include/prt_tick_external.h */
typedef bool (*CheckTickProcessFunc)(U32 coreID);
/* From: src/core/kernel/include/prt_task_external.h */
typedef void (*TaskNameGetFunc)(U32 taskId, char **taskName);
/* From: src/include/uapi/prt_task.h */
typedef U32 TskHandle;
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

/* From: src/include/uapi/prt_sys.h */
typedef U32 (*CoreWakeUpHook)(U32 coreId);
/* From: src/include/uapi/prt_hook.h */
struct HookModInfo {
    U8 maxNum[(U32)OS_HOOK_TYPE_NUM];
};

/* From: src/include/uapi/prt_err.h */
typedef void (*ErrHandleFunc)(const char *fileName, U32 lineNo, U32 errorNo, U32 paraLen, void *para);
/* From: src/core/kernel/include/prt_task_external.h */
typedef U32 (*TaskNameAddFunc)(U32 taskId, const char *name);
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

/* From: src/include/uapi/prt_sys.h */
typedef U64 (*SysTimeFunc)(void);
/* From: src/core/kernel/include/prt_task_external.h */
struct TagTskMonNode {
    /* 撑死/饿死的时间点(tick) */
    U64 expiredTick;
    /* 撑死/饿死标记 */
    U32 flag;
    /* 撑死/饿死检测标记 */
    U32 ckFlag;
    /* 检测类型 */
    U32 ckStyles;
    /* 保留(对齐) */
    U32 reserved;
};

/* From: src/core/kernel/include/prt_tick_external.h */
typedef U64 (*GetNearestTickFunc)(U32 coreID);
/* From: src/core/kernel/include/prt_rt_external.h */
struct RtActiveTskList
{
    /* 优先级bit位表 */
    U32 readyPrioBit[OS_GET_WORD_NUM_BY_PRIONUM(OS_TSK_NUM_OF_PRIORITIES)];
    struct TagListObject readyList[OS_TSK_NUM_OF_PRIORITIES];
};

/* From: src/core/kernel/include/prt_plist_external.h */
struct PushablTskList
{
    U32 prio;
    struct TagListObject nodeList;
};

/* From: src/core/kernel/include/prt_task_external.h */
struct TagOsTskSortedDelayList {
    /* 超时链涉及核间操作，必须有锁*/
    volatile uintptr_t spinLock;
    U32 reserved;
    /* 延时任务链表 */
    struct TagListObject tskList;
    /* 延时链上最近超时的tick刻度 */
    U64 nearestTicks;
};

/* From: src/include/uapi/prt_signal.h */
typedef struct {
    signalInfo siginfo;
    struct TagListObject siglist;
} sigInfoNode;
/* From: src/core/kernel/include/prt_plist_external.h */
struct PushablTskListHead
{
    struct TagListObject nodeList;
};

/* From: src/om/include/prt_hook_external.h */
typedef void *(*MemAllocHook)(enum MoudleId mid, U8 ptNo, U32 size);
/* From: src/include/uapi/prt_sem.h */
struct SemInfo {
    /* 信号量计数 */
    U32 count;
    /* 信号量占用者，对于计数型信号量，记录的是最后一次信号量获得者；如果没有被任务获得，则为OS_THREAD_ID_INVALID */
    U32 owner;
    /* 信号量唤醒方式，为SEM_MODE_FIFO或SEM_MODE_PRIOR */
    enum SemMode mode;
    /* 信号量类型，为SEM_TYPE_COUNT（计数型）或SEM_TYPE_BIN（互斥型） */
    U32 type;
};

/* From: src/om/include/prt_hook_external.h */
typedef U32(*OsHookChgFunc)(U32 hookType, enum HookChgType chgType);
/* From: src/core/kernel/include/prt_irq_external.h */
struct TagHwiHandleForm {
    union {
        HwiProcFunc hook;               // 非私有中断时为正常hook
        struct TagHwiHandleForm *form;  // 私有中断时为指向私有form数组的指针
    };
#if defined(OS_OPTION_HWI_ATTRIBUTE)
#endif
    HwiArg param;

};

struct TagHwiModeForm {
    HwiMode mode;
    HwiPrior prior;
#if defined(OS_OPTION_HWI_AFFINITY)
#endif
    U32 affinityMask;

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

typedef U32(*TskSwitchHook)(TskHandle lastTaskPid, TskHandle nextTaskPid);
typedef U32(*TskDeleteHook)(TskHandle taskPid);
typedef U32(*TskCreateHook)(TskHandle taskPid);
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
#endif
    U8 coreRunNum;
    /* 最大支持的核数*/
    U8 coreMaxNum;
    /* 主核ID */
    U8 corePrimary;

#if defined(OS_OPTION_HWI_MAX_NUM_CONFIG)
#endif
    U32 hwiMaxNum;

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
#ifndef STRUCT_TAGOSRUNQUE_DEFINED
#define STRUCT_TAGOSRUNQUE_DEFINED
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
#endif

/* From: src/core/kernel/include/prt_task_external.h */
struct TagScheduleClass {
    void (*osEnqueueTask)(struct TagOsRunQue *runQue, struct TagTskCb *tsk, U32 flags);
    void (*osDequeueTask)(struct TagOsRunQue *runQue, struct TagTskCb *tsk, U32 flags);
    void (*osPutPrevTask)(struct TagOsRunQue *runQue, struct TagTskCb *prevTsk);
    struct TagTskCb *(*osPickNextTask)(struct TagOsRunQue *runQue);
    struct TagTskCb *(*osNextReadyTask)(struct TagOsRunQue *runQue);
};

struct TagTskCb {
    /* 当前任务的SP */
    void *stackPointer;
    /* 任务状态,后续内部全改成U32 */
    U32 taskStatus;
#if defined(OS_OPTION_RR_SCHED)
    /* 任务得到调度的时间 */
    U64 startTime;
#if defined(OS_OPTION_RR_SCHED_IRQ_TIME_DISCOUNT)
    /* 中断处理消耗时间 */
#endif
    U64 irqUsedTime;


    /* 任务的运行优先级，数字越小优先级越高 */
#endif
    TskPrior priority;
    /* 任务栈配置标记 */
    U16 stackCfgFlg;
#if defined(OS_OPTION_SMP)
    /* 任务正在进行的操作类型 */
#endif
    volatile U32 taskOperating;
    /* 用于任务做关键不可打断的操作时不被打断如绑定，挂起，删任务，被删。0可以做这些事，为1不能做*/
    U32 opBusy;

    /* 任务栈大小 */
    U32 stackSize;
    TskHandle taskPid;
    /* 任务栈顶 */
    uintptr_t topOfStack;
    /* 任务入口函数 */
    TskEntryFunc taskEntry;
    /* 任务信号量Pend的信号量指针，或任务队列pend的队列指针 */
    void *taskPend;
    /* 任务的参数 */
    uintptr_t args[4];
#if (defined(OS_OPTION_TASK_INFO))
    /* 存放任务名 */
#endif
    char name[OS_TSK_NAME_LEN];

    /* 任务原始优先级 */
    TskPrior origPriority;
#if defined(OS_OPTION_RR_SCHED)
    /* 调度策略 */
#endif
    U16 policy;
    /* 剩余时间片 */
    U32 timeSlice;
#else
    /* 字节对齐 */
    U16 resv;

    /* 信号量链表指针 */
    struct TagListObject pendList;
    /* 任务延时链表指针 */
    struct TagListObject timerList;
    /* 持有互斥信号量链表 */
    struct TagListObject semBList;
    /* 记录条件变量的等待线程 */
    struct TagListObject condNode;
#if defined(OS_OPTION_LINUX)
    /* 等待队列指针 */
#endif
    struct TagListObject waitList;

#if defined(OS_OPTION_EVENT)
    /* 任务事件 */
#endif
    U32 event;
    /* 任务事件掩码 */
    U32 eventMask;

    /* 任务记录的最后一个错误码 */
    U32 lastErr;
#if defined(OS_OPTION_SMP)
    /* reserved */
#endif
    U32 smpReserve;
    struct PushablTskList pushAbleList;
    U32 timeCoreID;
    /* 该任务可以执行的核bitmap */
    OS_CORE_MASK coreAllowedMask;
    /* 该任务可以执行的核个数 */
    U32 nrCoresAllowed;
    /* 该任务所处的rq挂接的核号 */
    U32 coreID;
    /* 该任务是否在运行队列上(ready) */
    bool isOnRq;
    /* 该任务使用的调度类 */
    struct TagScheduleClass *scheClass;

    /* 任务恢复的时间点(单位Tick) */
    U64 expirationTick;
#if defined(OS_OPTION_POSIX)
    /* 当前任务状态 */
#endif
    U8 state;
    /* pthread cancel */
    U8 cancelState;
    U8 cancelType;
    U8 cancelPending;
    struct __ptcb *cancelBuf;
    /* exit status */
    void *retval;
    /* count for thread join */
    U16 joinCount;
    /* semaphore for thread join */
    SemHandle joinableSem;
    /* pthread key */
    void *tsd[PTHREAD_KEYS_MAX];
    U32 tsdUsed;
#if defined(OS_OPTION_POSIX_SIGNAL)
    /* 设置的阻塞信号掩码 */
#endif
    signalSet sigMask;
    /* 设置的等待信号掩码 */
    signalSet sigWaitMask;
    /* 未决信号掩码 */
    signalSet sigPending;
    /* 信号信息 */
    struct TagListObject sigInfoList;
    /* 信号处理函数 */
    _sa_handler sigVectors[PRT_SIGNAL_MAX];
    /* 保存任务的原SP */
    void *oldStackPointer;
    int holdSignal;
    timer_t itimer;

#if defined(OS_OPTION_LOCALE)
#endif
    locale_t locale;

#if defined(OS_OPTION_LINUX)
#endif
    struct task_struct *kthreadTsk;


#if defined(OS_OPTION_NUTTX_VFS)
#endif
    struct filelist tskFileList;
    void *stdio_locks;      // 来自musl中的struct pthread
#if defined(CONFIG_FILE_STREAM)
#endif
    struct streamlist ta_streamlist;


#if defined(OS_OPTION_PERF)
#endif
    uintptr_t pc;
    uintptr_t fp;

};

typedef U32 (*OsCheckPrioritySetFunc)(struct TagTskCb *taskCB, TskPrior taskPrio);
/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))
/* From: src/core/kernel/include/prt_sys_external.h */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))
/* From: src/arch/include/prt_cpu_external.h */
#define ASM_NOP() 
/* From: src/utility/lib/include/prt_lib_external.h */
#define ATTR_ALIGN_128 __attribute__((aligned(128))) /* 表示128 字节对齐 */
#define ATTR_ALIGN_16 __attribute__((aligned(16)))   /* 表示16 字节对齐 */
#define ATTR_ALIGN_32 __attribute__((aligned(32)))   /* 表示32 字节对齐 */
#define ATTR_ALIGN_4 __attribute__((aligned(4)))     /* 表示4字节对齐 */
#define ATTR_ALIGN_64 __attribute__((aligned(64)))   /* 表示64 字节对齐 */
#define ATTR_ALIGN_8 __attribute__((aligned(8)))     /* 表示8字节对齐 */
#define BNUM_TO_WNUM(bnum) (((bnum) + 31) >> 5)
/* From: src/core/kernel/include/prt_task_external.h */
#define CHECK_TSK_PID(taskID) (TSK_GET_INDEX(taskID) >= g_tskMaxNum)
#define CHECK_TSK_PID_OVERFLOW(taskId) (TSK_GET_INDEX(taskId) >= (g_tskMaxNum + 1))
/* From: src/core/kernel/include/prt_list_external.h */
#define COMPLEX_OF(ptr, type, field) ((type *)((uintptr_t)(ptr) - OFFSET_OF_FIELD(type, field)))
/* From: src/include/uapi/prt_task.h */
#define COMPOSE_PID(coreid, handle) 
/* From: src/utility/lib/include/prt_lib_external.h */
#define CPUMASK_AND(m1, m2) ((m1) & (m2))
#define CPUMASK_BCLR(mask ,bit) ((mask) &= ~(1U << (bit)))
#define CPUMASK_BSET(mask ,bit) ((mask) |= (1U << (bit)))
/* From: src/core/kernel/include/prt_task_external.h */
#define CPUMASK_FIRST_BIT(mask) OsGetRMB(mask)
/* From: src/utility/lib/include/prt_lib_external.h */
#define CPUMASK_HAS_BIT(mask ,bit) ((mask) & (1U << (bit)))
/* From: src/core/kernel/include/prt_sched_external.h */
#define CPU_AGENT_CPU(cpu) (cpu)
/* From: src/core/kernel/include/prt_task_external.h */
#define CPU_OVERTIME_SORT_LIST_LOCK(tskDlyBase) OS_MCMUTEX_LOCK(0, &(tskDlyBase)->spinLock)
#define CPU_OVERTIME_SORT_LIST_UNLOCK(tskDlyBase) OS_MCMUTEX_UNLOCK(0, &(tskDlyBase)->spinLock)
#define CPU_TSK_DELAY_BASE(cpu) (&g_tskSortedDelay[(cpu)])
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
/* From: src/core/kernel/include/prt_sched_external.h */
#define GET_RUNQ(core) ((struct TagOsRunQue *)&g_runQueue[(core)])
/* From: src/core/kernel/include/prt_task_external.h */
#define GET_TASK_RQ(task) (GET_RUNQ((task)->coreID))
#define GET_TCB_HANDLE(taskPid) (GET_TCB_HANDLE_BY_TCBID(TSK_GET_INDEX((uintptr_t)(taskPid))))
#define GET_TCB_HANDLE_BY_TCBID(index) (((struct TagTskCb *)g_tskCbArray) + (uintptr_t)(index))
#define GET_TCB_PEND(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)
/* From: src/core/kernel/include/prt_rt_external.h */
#define GET_TCB_READY(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)
/* From: src/om/include/prt_hook_external.h */
#define HOOK_ADD_IRQ_LOCK(intSave) 
#define HOOK_ADD_IRQ_UNLOCK(intSave) PRT_HwiRestore((intSave))
#define HOOK_DEL_IRQ_LOCK(intSave) 
#define HOOK_DEL_IRQ_UNLOCK(intSave) PRT_HwiRestore((intSave))
/* From: src/core/kernel/include/prt_task_external.h */
#define IDLE_TASK_ID g_idleTaskId
/* From: src/core/kernel/include/prt_list_external.h */
#define INIT_LIST_OBJECT(object) 
/* From: src/include/uapi/prt_typedef.h */
#define INLINE static __inline __attribute__((always_inline))
/* From: src/core/kernel/include/prt_task_external.h */
#define KTHREAD_TSK_STATE_SET(tsk, tskState) ((tsk)->kthreadTsk != NULL ? (tsk)->kthreadTsk->state = (tskState) : 0)
#define KTHREAD_TSK_STATE_TST(tsk, tskState) (((tsk)->kthreadTsk != NULL) && ((tsk)->kthreadTsk->state == (tskState)))
/* From: src/utility/lib/include/prt_lib_external.h */
#define LEB128_MAX_VALUE 0x7FU
#define LEB128_STEP_SIZE 0x7U
/* From: src/include/uapi/prt_typedef.h */
#define LIKELY(x) __builtin_expect(!!(x), 1)
/* From: src/core/kernel/include/prt_list_external.h */
#define LIST_COMPONENT(ptrOfList, typeOfList, fieldOfList) COMPLEX_OF(ptrOfList, typeOfList, fieldOfList)
#define LIST_FIRST_ENTITY(listForFirstEntity, typeOfList, field) 
#define LIST_FOR_EACH(posOfList, listObject, typeOfList, field) 
#define LIST_FOR_EACH_SAFE(posOfList, listObject, typeOfList, field) 
#define LIST_LAST(object) ((object)->prev)
#define LIST_OBJECT_INIT(object) {
/* From: src/om/include/prt_err_external.h */
#define LOG_ADDR_DBG(addr) ((void)(addr), 0x0U)
/* From: src/utility/lib/include/prt_lib_external.h */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
/* From: src/core/kernel/include/prt_task_external.h */
#define MAX_TASK_NUM ((1U << OS_TSK_TCB_INDEX_BITS) - 2)  // 254
/* From: src/utility/lib/include/prt_lib_external.h */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
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
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_32BIT_MASK(bit) (1U << OS_32BIT_MOD(bit))
#define OS_32BIT_MOD(num) ((num) % 32)
#define OS_32BIT_VERSE_MASK(bit) (~(1U << OS_32BIT_MOD(bit)))
#define OS_64BIT_SET(high, low) (((U64)(high) << 32) + (U64)(low))
#define OS_ADDR_OVERTURN_CHK(addr, size) (((size) != 0) && (((addr) + (size) - 1) < (addr)))
#define OS_ALIGN(addr, align) ((uintptr_t)(addr) & ((align) - 1))
#define OS_ALIGN_CHK(addr, align) (((uintptr_t)(addr) & ((align) - 1)) == 0)
#define OS_BIT0_MASK (0x1U << 0)
#define OS_BIT10_MASK (0x1U << 10)
#define OS_BIT11_MASK (0x1U << 11)
#define OS_BIT12_MASK (0x1U << 12)
#define OS_BIT13_MASK (0x1U << 13)
#define OS_BIT14_MASK (0x1U << 14)
#define OS_BIT15_MASK (0x1U << 15)
#define OS_BIT16_MASK (0x1U << 16)
#define OS_BIT17_MASK (0x1U << 17)
#define OS_BIT18_MASK (0x1U << 18)
#define OS_BIT19_MASK (0x1U << 19)
#define OS_BIT1_MASK (0x1U << 1)
#define OS_BIT20_MASK (0x1U << 20)
#define OS_BIT21_MASK (0x1U << 21)
#define OS_BIT22_MASK (0x1U << 22)
#define OS_BIT23_MASK (0x1U << 23)
#define OS_BIT24_MASK (0x1U << 24)
#define OS_BIT25_MASK (0x1U << 25)
#define OS_BIT26_MASK (0x1U << 26)
#define OS_BIT27_MASK (0x1U << 27)
#define OS_BIT28_MASK (0x1U << 28)
#define OS_BIT29_MASK (0x1U << 29)
#define OS_BIT2_MASK (0x1U << 2)
#define OS_BIT30_MASK (0x1U << 30)
#define OS_BIT31_MASK (0x1U << 31)
#define OS_BIT3_MASK (0x1U << 3)
#define OS_BIT4_MASK (0x1U << 4)
#define OS_BIT5_MASK (0x1U << 5)
#define OS_BIT6_MASK (0x1U << 6)
#define OS_BIT7_MASK (0x1U << 7)
#define OS_BIT8_MASK (0x1U << 8)
#define OS_BIT9_MASK (0x1U << 9)
#define OS_BITNUM_2_MASK(bitNum) ((1U << (bitNum)) - 1)
#define OS_BITS_PER_BYTE 8
#define OS_BIT_SET_VALUE(bit) (0x1UL << (bit))
/* From: src/include/uapi/prt_sys.h */
#define OS_BOOT_PHASE 0x03
#define OS_BSSINIT_PHASE 0x06
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_BYTES_PER_DWORD (sizeof(U64))
#define OS_BYTES_PER_WORD (sizeof(U32))
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_CLR_CHILD_BIT_MAP(priority) 
#define OS_CLR_RDY_TSK_BIT_MAP(priority) 
#define OS_COREID_CHK_INVALID(coreId) ((coreId) >= g_maxNumOfCores)
#define OS_CORES_32 32
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_CORE_STR_END_INDEX (OS_CORE_STR_MAX_LEN - 1)   // 结束字符位置索引
#define OS_CORE_STR_NUM_INDEX (OS_CORE_STR_END_INDEX -1)  // 最后一个有效字符位置索引
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_DECIMAL 10
/* From: src/include/uapi/prt_sys.h */
#define OS_DEFAULT_PHASE 0x00
/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_DEFAULT_SCHED_TYPE OS_SCHEDULE_RT_SINGLE
/* From: src/include/uapi/prt_sys.h */
#define OS_DEVDRVINIT_PHASE 0x0b
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_DWORD_BIT_NUM 64
#define OS_DWORD_BIT_POW 6
/* From: src/include/uapi/prt_typedef.h */
#define OS_EMBED_ASM __asm__ __volatile__
/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_ENQUEUE_HEAD 1U /* 入队加头 */
/* From: src/include/uapi/prt_errno.h */
#define OS_ERRNO_BUILD_ERROR(mid, errno) (ERRTYPE_ERROR | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
#define OS_ERRNO_BUILD_FATAL(mid, errno) (ERRTYPE_FATAL | ERRNO_OS_ID | ((U32)(mid) << 8) | (errno))
/* From: src/include/uapi/prt_task.h */
#define OS_ERRNO_BUILD_ID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_TSK, 0x23)
/* From: src/include/uapi/prt_mem.h */
#define OS_ERRNO_FSCMEM_ALLOC_NO_MEMORY OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x0c)
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
/* From: src/include/uapi/prt_mem.h */
#define OS_ERRNO_MEM_ALLOC_ALIGNPOW_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x0a)
#define OS_ERRNO_MEM_ALLOC_SIZETOOLARGE OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x04)
#define OS_ERRNO_MEM_ALLOC_SIZE_ZERO OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x09)
#define OS_ERRNO_MEM_FREE_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x02)
#define OS_ERRNO_MEM_FREE_SH_DAMAGED OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x0b)
#define OS_ERRNO_MEM_INITADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x06)
#define OS_ERRNO_MEM_INITADDR_ISINVALID OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x03)
#define OS_ERRNO_MEM_INITSIZE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x05)
#define OS_ERRNO_MEM_OVERWRITE OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x01)
#define OS_ERRNO_MEM_PTCREATE_SIZE_ISTOOBIG OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x08)
#define OS_ERRNO_MEM_PTCREATE_SIZE_ISTOOSMALL OS_ERRNO_BUILD_ERROR(OS_MID_MEM, 0x07)
/* From: src/include/uapi/prt_hwi.h */
#define OS_ERRNO_MULTI_TARGET_CORE OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x10)
/* From: src/include/uapi/prt_sem.h */
#define OS_ERRNO_SEM_ALL_BUSY OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x04)
#define OS_ERRNO_SEM_COUNT_GET_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x0f)
#define OS_ERRNO_SEM_INFO_NULL OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x0e)
#define OS_ERRNO_SEM_INPUT_BUF_NOT_ENOUGH OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x0c)
#define OS_ERRNO_SEM_INPUT_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x0d)
#define OS_ERRNO_SEM_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x02)
#define OS_ERRNO_SEM_MUTEX_HOLDING OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x12)
#define OS_ERRNO_SEM_MUTEX_NOT_OWNER_POST OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x10)
#define OS_ERRNO_SEM_MUTEX_POST_INTERR OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x11)
#define OS_ERRNO_SEM_NO_MEMORY OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x01)
#define OS_ERRNO_SEM_OVERFLOW OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x09)
#define OS_ERRNO_SEM_PENDED OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x0a)
#define OS_ERRNO_SEM_PEND_INTERR OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x06)
#define OS_ERRNO_SEM_PEND_IN_LOCK OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x07)
#define OS_ERRNO_SEM_PTR_NULL OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x03)
#define OS_ERRNO_SEM_REG_ERROR OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x0b)
#define OS_ERRNO_SEM_TIMEOUT OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x08)
#define OS_ERRNO_SEM_UNAVAILABLE OS_ERRNO_BUILD_ERROR(OS_MID_SEM, 0x05)
/* From: src/include/uapi/prt_signal.h */
#define OS_ERRNO_SIGNAL_NO_MEMORY OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x08)
#define OS_ERRNO_SIGNAL_NUM_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x07)
#define OS_ERRNO_SIGNAL_PARA_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x01)
#define OS_ERRNO_SIGNAL_TASKID_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x05)
#define OS_ERRNO_SIGNAL_TIMEOUT OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x02)
#define OS_ERRNO_SIGNAL_TSK_NOT_CREATED OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x06)
#define OS_ERRNO_SIGNAL_WAIT_IN_LOCK OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x04)
#define OS_ERRNO_SIGNAL_WAIT_NOT_IN_TASK OS_ERRNO_BUILD_ERROR(OS_MID_SIGNAL, 0x03)
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
/* From: src/include/uapi/prt_hwi.h */
#define OS_ERROR_HWI_BASE_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x12)
#define OS_ERROR_HWI_INT_LOCK_REG_PARA_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_HWI, 0x13)
#define OS_ERROR_HWI_INT_REGISTER_FAILED OS_ERRNO_BUILD_FATAL(OS_MID_HWI, 0x0e)
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
/* From: src/arch/include/prt_asm_cpu_external.h */
#define OS_FLG_BGD_ACTIVE 0x0002
#define OS_FLG_EXC_ACTIVE 0x0020
#define OS_FLG_HWI_ACTIVE 0x0001
#define OS_FLG_SYS_ACTIVE 0x0010
#define OS_FLG_TICK_ACTIVE 0x0008
/* From: src/mem/include/prt_mem_external.h */
#define OS_FSC_MEM_LAST_IDX 31
#define OS_FSC_MEM_MAGIC_USED (struct TagFscMemCtrl *)0x5a5aa5a5
#define OS_FSC_MEM_MAXVAL ((1U << OS_FSC_MEM_LAST_IDX) - OS_FSC_MEM_SIZE_ALIGN)
/* From: src/mem/fsc/prt_fscmem_internal.h */
#define OS_FSC_MEM_SIZE_ALIGN OS_MEM_ADDR_ALIGN
/* From: src/mem/include/prt_mem_external.h */
#define OS_FSC_MEM_SZGET(currBlk) ((U32)(currBlk->size))
#define OS_FSC_MEM_TAIL_MAGIC 0xABCDDCBA
#define OS_FSC_MEM_TAIL_SIZE (sizeof(uintptr_t))
#define OS_FSC_MEM_USED_HEAD_SIZE (sizeof(struct TagFscMemCtrl))
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_GET_16BIT_HIGH_8BIT(num) ((num) >> 8)
#define OS_GET_32BIT_ARRAY_BASE(num) ((num) << 5)
#define OS_GET_32BIT_ARRAY_INDEX(num) ((num) >> 5)
#define OS_GET_32BIT_HIGH_16BIT(num) ((num) >> 16)
#define OS_GET_32BIT_HIGH_8BIT(num) (((num) >> 24) & 0xffU)
#define OS_GET_32BIT_LOW_16BIT(num) ((num) & 0xFFFFU)
#define OS_GET_32BIT_LOW_8BIT(num) ((num) & 0xffU)
#define OS_GET_64BIT_HIGH_32BIT(num) ((U32)((num) >> 32))
#define OS_GET_64BIT_LOW_32BIT(num) ((U32)((num) & 0xFFFFFFFFU))
#define OS_GET_8BIT_HIGH_4BIT(num) ((num) >> 4)
#define OS_GET_8BIT_LOW_4BIT(num) ((num) & 0xFU)
#define OS_GET_BIT_IN_WORD(num) ((num) & 0x1FUL)
/* From: src/arch/include/prt_cpu_external.h */
#define OS_GOTO_SYS_ERROR() 
#define OS_GOTO_SYS_ERROR1() OS_GOTO_SYS_ERROR()
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_HALF_WORD_BIT_NUM 16
#define OS_HEX 16
/* From: src/om/include/prt_hook_external.h */
#define OS_HOOK_CPUP_WARN 4
#define OS_HOOK_ERR_REG 5
#define OS_HOOK_FIRST_TIME_SWH 2
#define OS_HOOK_IDLE_PREFIX 6
#define OS_HOOK_MEM_DAMAGE 7
#define OS_HOOK_TICK_ENTRY 0
#define OS_HOOK_TICK_EXIT 1
#define OS_HOOK_TSK_MON 3
#define OS_HOOK_TYPE_TOTAL 8
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_HWI_ACTIVE ((UNI_FLAG & OS_HWI_ACTIVE_MASK) != 0)
#define OS_HWI_ACTIVE_MASK (OS_FLG_HWI_ACTIVE | OS_FLG_TICK_ACTIVE | OS_FLG_SYS_ACTIVE | OS_FLG_EXC_ACTIVE)
/* From: src/include/uapi/prt_hwi.h */
#define OS_HWI_ATTR(mode, type) (HwiMode)((HwiMode)(mode) | (HwiMode)(type))
/* From: src/include/uapi/prt_task.h */
#define OS_HWI_HANDLE ((1U << OS_TSK_TCB_INDEX_BITS) - 1)
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_HWI_INACTIVE (!(OS_INT_ACTIVE))
/* From: src/core/kernel/include/prt_irq_external.h */
#define OS_HWI_INVALID_TCB_ADDR 0xffffffff
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
/* From: src/arch/include/prt_hwi_external.h */
#define OS_HWI_NUM_MASK 0x1FU
/* From: src/include/uapi/prt_hwi.h */
#define OS_HWI_TYPE_NORMAL 0x00
/* From: src/include/uapi/prt_sys.h */
#define OS_INITIALIZE_PHASE 0x0a
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_INT_ACTIVE ((UNI_FLAG & OS_INT_ACTIVE_MASK) != 0)
#define OS_INT_ACTIVE_MASK
/* From: src/core/kernel/include/prt_irq_external.h */
#define OS_INT_COUNT g_intCount
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_INT_INACTIVE (!(OS_INT_ACTIVE))
/* From: src/include/uapi/prt_typedef.h */
#define OS_INVALID (-1)
/* From: src/include/uapi/prt_atomic.h */
#define OS_INVALID_LOCK_OWNER_ID 0xFFFFFFFF
/* From: src/include/uapi/prt_sem.h */
#define OS_INVALID_OWNER_ID 0xFFFFFFFE
/* From: src/core/kernel/include/prt_irq_external.h */
/* #define OS_IRQ_TIME_RECORD(irqStartTime) (void)irqStartTime - defined in test_support.c */
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_IS_VISIBLE_CHAR(word) ((((word) >= 'a') && ((word) <= 'z')) || (((word) >= 'A') && ((word) <= 'Z')) ||
/* From: src/include/uapi/prt_sys.h */
#define OS_LIBCINIT_PHASE 0x08
/* From: src/core/kernel/include/prt_list_external.h */
#define OS_LIST_FIRST(object) ((object)->next)
#define OS_LIST_INIT(head) 
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_LMB32 31
/* From: src/include/uapi/prt_atomic.h */
#define OS_LOCK_OWNER_SYS 0xFFFFFFFF
/* From: src/om/include/prt_err_external.h */
#define OS_LOG_LEVEL_FORCE 0xFF
#define OS_LOG_REPORT_DBG(format, ...) 
/* From: src/include/uapi/prt_buildef.h */
#define OS_MAX_CORE_NUM 4
/* From: src/mem/include/prt_mem_external.h */
#define OS_MAX_PT_NUM 253
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_MAX_S8 0xFF
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_MAX_TCB_NUM (g_tskMaxNum + 1 + 1)  // 1个IDLE，1个无效任务
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_MAX_U12 0xFFFU
#define OS_MAX_U16 0xFFFFU
#define OS_MAX_U32 0xFFFFFFFFU
#define OS_MAX_U4 0xFU
#define OS_MAX_U8 0xFFU
/* From: src/arch/include/prt_raw_spinlock_external.h */
#define OS_MCMUTEX_IRQ_LOCK(hwSemId, addr, intSave) 
#define OS_MCMUTEX_IRQ_UNLOCK(hwSemId, addr, intSave) 
#define OS_MCMUTEX_LOCK(hwSemId, addr) 
#define OS_MCMUTEX_UNLOCK(hwSemId, addr) 
/* From: src/include/uapi/prt_mem.h */
#define OS_MEM_ADDR_ALIGN sizeof(uintptr_t)
/* From: src/mem/include/prt_mem_external.h */
#define OS_MEM_ALIGN_CHECK(value) ((value) & 0x3UL)
/* From: src/include/uapi/prt_mem.h */
#define OS_MEM_DEFAULT_FSC_PT (OS_MEM_DEFAULT_PT0)
#define OS_MEM_DEFAULT_PT0 0
#define OS_MEM_DEFAULT_PT1 1
#define OS_MEM_DEFAULT_PTNUM 2
/* From: src/mem/include/prt_mem_external.h */
#define OS_MEM_HEAD_MAGICWORD 0xABAB /* 内存块的块控制头魔术字，确保为奇数 */
/* From: src/om/include/prt_hook_external.h */
#define OS_MHOOK_ACTIVATE(hookType, funcType, list) 
#define OS_MHOOK_ACTIVATE_PARA0(hookType) OS_MHOOK_ACTIVATE((hookType), OsFunPara0, pfn())
#define OS_MHOOK_ACTIVATE_PARA1(hookType, arg0) OS_MHOOK_ACTIVATE((hookType), OsFunPara1, pfn((uintptr_t)(arg0)))
#define OS_MHOOK_ACTIVATE_PARA2(hookType, arg0, arg1) OS_MHOOK_ACTIVATE((hookType),
#define OS_MHOOK_ACTIVATE_PARA3(hookType, arg0, arg1, arg2) OS_MHOOK_ACTIVATE((hookType),
#define OS_MHOOK_ACTIVATE_PARA4(hookType, arg0, arg1, arg2, arg3) 
#define OS_MHOOK_ACTIVATE_PARA5(hookType, arg0, arg1, arg2, arg3, arg4) 
#define OS_MHOOK_ACTIVATE_PROC(hook, funcType, pfn, list) 
#define OS_MHOOK_BOUNDARY ((U32)1)
#define OS_MHOOK_IS_VALID(hook) ((uintptr_t)(hook) > OS_MHOOK_BOUNDARY)
#define OS_MHOOK_NODE_DEAD ((OsVoidFunc)2)
#define OS_MHOOK_NOT_BOUNDARY(hook) ((uintptr_t)(hook) != OS_MHOOK_BOUNDARY)
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_MS2CYCLE(ms, clock) DIV64(((ms) * (U64)(clock)), OS_SYS_MS_PER_SECOND) /* 毫秒转换成cycle */
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_NOT_ALIGN_CHK(addr, align) (!OS_ALIGN_CHK(addr, align))
/* From: src/include/uapi/prt_sem.h */
#define OS_NO_WAIT 0
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_OCTAL 8
/* From: src/include/uapi/prt_typedef.h */
#define OS_OK 0
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_PST_ZOMBIE_TASK (GET_TCB_HANDLE_BY_TCBID(OS_MAX_TCB_NUM - 1 ))
/* From: src/include/uapi/prt_sys.h */
#define OS_REGISTER_PHASE 0x09
/* From: src/om/include/prt_err_external.h */
#define OS_REPORT_ERROR(errNo) 
/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_RT_MAX_TRYS 3
#define OS_SCHED_CLASS(coreID) (GET_RUNQ(coreID)->schedClass)
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
/* From: src/include/uapi/prt_sem.h */
#define OS_SEM_COUNT_MAX 0xFFFFFFFE
#define OS_SEM_EMPTY 0
#define OS_SEM_FULL 1
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_SET_CHILD_BIT_MAP(priority) 
#define OS_SET_RDY_TSK_BIT_MAP(priority) 
/* From: src/om/include/prt_hook_external.h */
#define OS_SHOOK_ACTIVATE(hookType, funcType, list) 
#define OS_SHOOK_ACTIVATE_PARA0(hookType) OS_SHOOK_ACTIVATE((hookType), OsFunPara0, pfn())
#define OS_SHOOK_ACTIVATE_PARA1(hookType, arg0) OS_SHOOK_ACTIVATE((hookType), OsFunPara1, pfn((uintptr_t)(arg0)))
#define OS_SHOOK_ACTIVATE_PARA2(hookType, arg0, arg1) OS_SHOOK_ACTIVATE((hookType),
#define OS_SHOOK_ACTIVATE_PARA3(hookType, arg0, arg1, arg2) OS_SHOOK_ACTIVATE((hookType),
#define OS_SHOOK_ACTIVATE_PARA4(hookType, arg0, arg1, arg2, arg3) 
#define OS_SHOOK_ACTIVATE_PARA5(hookType, arg0, arg1, arg2, arg3, arg4) 
#define OS_SHOOK_TYPE_START ((U32)OS_HOOK_TSK_MON)
/* From: src/include/uapi/prt_signal.h */
#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFF
/* From: src/core/kernel/include/prt_sched_external.h */
#define OS_SMP_HANDSHAKE_COUNT(core) (GET_RUNQ(core)->shakeCount)
/* From: src/arch/cpu/armv7-r/common/os_cpu_armv7_r_external.h */
#define OS_SMP_SCHED_TRIGGER_OTHER_CORE_SGI OS_HWI_IPI_NO_01 // 触发它核响应一次调度的IPI中断号
/* From: src/include/uapi/prt_atomic.h */
#define OS_SPINLOCK_LOCK 1
#define OS_SPINLOCK_UNLOCK 0
/* From: src/arch/include/prt_raw_spinlock_external.h */
#define OS_SPIN_LOCK(lockVar) OsSplLock((volatile uintptr_t *)(lockVar))
#define OS_SPIN_TRY_LOCK(lockVar) OsSplTryLock((uintptr_t *)(lockVar))
#define OS_SPIN_UNLOCK(lockVar) OsSplUnlock((volatile uintptr_t *)(lockVar))
/* From: src/include/uapi/prt_sys.h */
#define OS_START_PHASE 0x0c
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_STATIC_NO_INLINE static __noinline __attribute__((noinline))
/* From: src/include/uapi/prt_sys.h */
#define OS_SYS_APP_VER_LEN 64
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_SYS_CONTEXT_PID 0xFFFFFFFFU
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
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_SYS_UINT_IDX(coreId) ((U32)(coreId) >> 5)  // 一个U32表示32个核
#define OS_SYS_UINT_LCOREID_OFFSET(coreId) (((coreId) & 0x1fUL))  // 核号在一个U32里的偏移
/* From: src/include/uapi/prt_sys.h */
#define OS_SYS_US_PER_SECOND 1000000
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TASK_GET_PRI_LIST(prio) (&((struct TagOsRunQue *)THIS_RUNQ())->rtRq.activeTsk.readyList[(prio)])
#define OS_TASK_LOCK_DATA g_uniTaskLock
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_TASK_SCAN() 
#define OS_THREAD_FLAG_MASK
/* From: src/core/kernel/include/prt_tick_external.h */
#define OS_TICKLESS_FOREVER ((U64)-1)
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_BLOCK (OS_TSK_DELAY | OS_TSK_PEND | OS_TSK_SUSPEND  | OS_TSK_QUEUE_PEND |
#define OS_TSK_COMPOSE(tId) (tId)
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_CRG_IDLE_SUSPEND 0X20000U
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_DEFAULT_PRIORITY 20
#define OS_TSK_DEFAULT_TIME_SLICE_MS 5
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_DELAY 0x0020
#define OS_TSK_DELAY_INTERRUPTIBLE 0x8000
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_DELAY_LOCKED_DETACH(task) ListDelete(&(task)->timerList)
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_DELETING 0X8000U
#define OS_TSK_EVENT_PEND 0x0800
#define OS_TSK_EVENT_TYPE 0x1000
#define OS_TSK_HOLD_SIGNAL 0x0400
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_ID_BITS_NUM 10
#define OS_TSK_INDEX_MASK ((1U << OS_TSK_ID_BITS_NUM) -1)
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_INUSE 0x0001
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_LOCK() (OS_TASK_LOCK_DATA++)
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_NULL_ID 0xFFFFFFFF
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_OP_CLR(taskCB, operate) ((void)(taskCB), (void)(operate))
#define OS_TSK_OP_DELETING (1UL << 9)  // 任务在删除中，必须等待上述迁移、挂起操作完
#define OS_TSK_OP_FREE 0
#define OS_TSK_OP_MIGRATING (1UL << 0)  // 任务在迁移中，看到delete立即返回
#define OS_TSK_OP_MOVING (1UL << 10) // 任务在迁移/回迁过程中标志，确保不会被他人操作该任务
#define OS_TSK_OP_RESUMING (1UL << 3)  // 任务在恢复中，看到delete立即返回
#define OS_TSK_OP_SET(taskCB, operate) ((void)(taskCB), (void)(operate))
#define OS_TSK_OP_SUSPENDING (1UL << 2)  // 任务在挂起中，看到delete立即返回
#define OS_TSK_PARA_0 0
#define OS_TSK_PARA_1 1
#define OS_TSK_PARA_2 2
#define OS_TSK_PARA_3 3
/* From: src/include/uapi/prt_task.h */
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
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_PRIO_BIT_MAP_POW 5
#define OS_TSK_PRIO_RDY_BIT 0x80000000U
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_QUEUE_BUSY 0x4000
#define OS_TSK_QUEUE_PEND 0x2000
#define OS_TSK_READY 0x0040
#define OS_TSK_RUNNING 0x0080
#define OS_TSK_RW_PEND 0x40000
#define OS_TSK_SCHED_FIFO 0x0U
#define OS_TSK_SCHED_RR 0x1U
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_SCHED_ZOMBIE 0xABAB
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_SET_PERIODIC 0x80000
#define OS_TSK_SIG_PAUSE 0x20000
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_STACK_CFG_BY_SYS 0
#define OS_TSK_STACK_CFG_BY_USER 1
#define OS_TSK_STACK_TOP_MAGIC 0xAAAAAAAA
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_SUSPEND 0x0004
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_SUSPEND_READY_BLOCK (OS_TSK_SUSPEND)
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_TCB_INDEX_BITS ((4 - OS_TSK_CORE_BYTES_IN_PID) * 8)
#define OS_TSK_TIMEOUT 0x0010
/* From: src/core/kernel/include/prt_task_external.h */
#define OS_TSK_UNLOCK() OsTskUnlock()
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_UNUSED 0x0000
#define OS_TSK_WAITQUEUE_PEND 0x10000
#define OS_TSK_WAIT_SIGNAL 0x0002
/* From: src/core/kernel/include/prt_sys_external.h */
#define OS_US2CYCLE(us, clock) DIV64(((us) * (U64)(clock)), OS_SYS_US_PER_SECOND) /* 微秒转换成cycle */
#define OS_VAR_ARRAY_NUM OS_MAX_CORE_NUM
/* From: src/include/uapi/prt_sem.h */
#define OS_WAIT_FOREVER 0xFFFFFFFF
/* From: src/utility/lib/include/prt_lib_external.h */
#define OS_WORD_ALLBIT_MASK 0xFFFFFFFFU
#define OS_WORD_BIT_NUM 32
#define OS_WORD_BIT_POW 5
/* From: src/arch/cpu/x86_64/common/os_cpu_x86_64_external.h */
#define OsSplLock(spinLock) 
#define OsSplUnlock(spinLock) 
/* From: src/include/uapi/prt_atomic.h */
#define PRT_ATOMIC_H
/* From: src/arch/include/prt_attr_external.h */
#define PRT_ATTR_EXTERNAL_H
/* From: src/arch/include/prt_cpu_external.h */
#define PRT_CPU_EXTERNAL_H
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/om/include/prt_err_external.h */
#define PRT_ERR_EXTERNAL_H
/* From: src/include/uapi/prt_err.h */
#define PRT_ERR_H
/* From: src/om/include/prt_hook_external.h */
#define PRT_HOOK_EXTERNAL_H
/* From: src/include/uapi/prt_hook.h */
#define PRT_HOOK_H
/* From: src/arch/include/prt_hwi_external.h */
#define PRT_HWI_EXTERNAL_H
/* From: src/include/uapi/prt_hwi.h */
#define PRT_HWI_H
/* From: src/core/kernel/include/prt_irq_external.h */
#define PRT_IRQ_EXTERNAL_H
/* From: src/utility/lib/include/prt_lib_external.h */
#define PRT_LIB_EXTERNAL_H
/* From: src/core/kernel/include/prt_list_external.h */
#define PRT_LIST_EXTERNAL_H
/* From: src/mem/include/prt_mem_external.h */
#define PRT_MEM_EXTERNAL_H
/* From: src/include/uapi/prt_mem.h */
#define PRT_MEM_H
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/core/kernel/include/prt_plist_external.h */
#define PRT_PLIST_EXTERNAL_H
/* From: src/arch/include/prt_raw_spinlock_external.h */
#define PRT_RAW_SPINLOCK_EXTERNAL_H
/* From: src/core/kernel/include/prt_rt_external.h */
#define PRT_RT_EXTERNAL_H
/* From: src/core/kernel/include/prt_sched_external.h */
#define PRT_SCHED_EXTERNAL_H
/* From: src/include/uapi/prt_sem.h */
#define PRT_SEM_H
/* From: src/include/uapi/prt_signal.h */
#define PRT_SIGNAL_H
/* From: src/core/kernel/include/prt_sys_external.h */
#define PRT_SYS_EXTERNAL_H
/* From: src/include/uapi/prt_sys.h */
#define PRT_SYS_H
/* From: src/core/kernel/include/prt_task_external.h */
#define PRT_TASK_EXTERNAL_H
/* From: src/include/uapi/prt_task.h */
#define PRT_TASK_H
/* From: src/core/kernel/include/prt_tick_external.h */
#define PRT_TICK_EXTERNAL_H
/* From: src/include/uapi/prt_tick.h */
#define PRT_TICK_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
/* From: src/core/kernel/include/prt_sched_external.h */
#define RT_SINGLE_CLASS() (&g_osRtSingleSchedClass)
/* From: src/core/kernel/include/prt_task_external.h */
/* #define RUNNING_TASK g_runningTask - defined in test_support.c */
/* From: src/include/uapi/prt_typedef.h */
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
/* From: platform/libboundscheck/include/securec.h */
#define SECUREC_H
/* From: src/include/uapi/prt_sem.h */
#define SEM_MUTEX_TYPE_NORMAL 0
#define SEM_MUTEX_TYPE_RECUR 1
#define SEM_TYPE_BIN 1
#define SEM_TYPE_COUNT 0
/* From: src/core/kernel/include/prt_sched_external.h */
#define SMP_MC_SCHEDULE_TRIGGER(core) OsHwiTriggerByMask((U32)(1UL << (core)), OS_SMP_SCHED_TRIGGER_OTHER_CORE_SGI)
#define SMP_MC_SCHEDULE_TRIGGER_SELF(core) OsHwiTriggerSelf(core, OS_HWI_IPI_NO_01)
/* From: src/core/kernel/include/prt_sys_external.h */
#define THIS_CORE() OS_THIS_CORE
/* From: src/core/kernel/include/prt_sched_external.h */
#define THIS_RUNQ() ((struct TagOsRunQue *)&g_runQueue[THIS_CORE()])
/* From: src/core/kernel/include/prt_sys_external.h */
/* #define TICK_NO_RESPOND_CNT g_tickNoRespondCnt - defined in test_support.c */
/* From: src/include/uapi/prt_task.h */
#define TM_INFINITE 0
#define TM_NOW 0
/* From: src/include/uapi/prt_typedef.h */
#define TRUE ((bool)1)
#define TRUNCATE(addr, size) ((addr) & ~((uintptr_t)(size) - 1))
/* From: src/core/kernel/include/prt_task_external.h */
#define TSK_CORE_GET(taskPID) (GET_TCB_HANDLE(taskPID)->coreID)
#define TSK_CORE_SET(tsk, coreId) ((tsk)->coreID = (coreId))
#define TSK_CREATE_DEL_LOCK() OS_MCMUTEX_LOCK(0, &g_createTskLock)
#define TSK_CREATE_DEL_UNLOCK() OS_MCMUTEX_UNLOCK(0, &g_createTskLock)
#define TSK_GET_INDEX(taskId) ((taskId) - g_tskBaseId)
#define TSK_IDX2PID(taskID) (taskID)    // ((taskID) + g_tskBaseID)
#define TSK_IS_UNUSED(tsk) ((tsk)->taskStatus == OS_TSK_UNUSED)
#define TSK_STATUS_CLEAR(tsk, statBit) ((tsk)->taskStatus &= ~(statBit))
#define TSK_STATUS_SET(tsk, statBit) ((tsk)->taskStatus |= (statBit))
#define TSK_STATUS_TST(tsk, statBit) (((tsk)->taskStatus & (statBit)) != 0)
/* From: src/include/uapi/prt_typedef.h */
#define U12_INVALID 0xfffU
#define U16_INVALID 0xffffU
#define U32_INVALID 0xffffffffU
#define U32_MAX 0xFFFFFFFFU
#define U64_INVALID 0xffffffffffffffffUL
#define U8_INVALID 0xffU
/* From: src/core/kernel/include/prt_sys_external.h */
/* #define UNI_FLAG (THIS_RUNQ()->uniFlag) - defined in test_support.c */
/* From: src/include/uapi/prt_typedef.h */
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define YES 1
/* From: src/include/uapi/prt_signal.h */
#define sigMask(signo) (1U << (signo - 1))
#define sigValid(signo) (signo > 0 && signo < PRT_SIGNAL_MAX)
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
/* Ready list operations - defined in test_support.c */
/* static inline void OsTskReadyDel(struct TagTskCb *tcb) { (void)tcb; } */
/* static inline void OsTskReadyAdd(struct TagTskCb *tcb) { (void)tcb; } */
/* Scheduler trigger */
static inline void OsTskSchedule(void) { }
/* Semaphore priority lock */
static inline void OsSemPrioLock(void) { }
static inline void OsSemPrioUnLock(void) { }
/* Check priority set (g_checkPrioritySet validation hook) */
/* OsCheckPrioritySet now from func_under_test.c */
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
/* static U32 g_uniTaskLock = 0; - defined in test_support.c */
/* static U32 g_tickNoRespondCnt = 0; - defined in test_support.c */
static U32 g_createTskLock = 0;
static struct TagTskCb *g_tskCbArray = NULL;
static struct TagOsRunQue g_runQueue[1];
static struct TagOsTskSortedDelayList g_tskSortedDelay[1];
/* static struct TagTskCb *g_runningTask = NULL; - defined in test_support.c */
static void *g_semTsk = NULL;
static void *g_tskSem = NULL;
static struct TagScheduleClass g_osRtSingleSchedClass = {0};
extern volatile int g_hwi_lock_count;
#endif /* AUTO_STUB_PRT_SCHED_SINGLE_H */
