/*
 * Auto-generated stub header for prt_perf_ringbuf.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/extended/perf/prt_perf_ringbuf.c
 * Included headers: 9
 */
#ifndef AUTO_STUB_PRT_PERF_RINGBUF_H
#define AUTO_STUB_PRT_PERF_RINGBUF_H
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
typedef U32 signalSet;
typedef void *timer_t;
typedef void *locale_t;
typedef void *_sa_handler;
struct filelist { void *dummy; };
struct streamlist { void *dummy; };
struct task_struct { void *dummy; };
struct __ptcb { void *dummy; };

/* ---- Structural macros (used in type definitions) ---- */
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_NAME_LEN 16
/* ---- Types ---- */
/* From: src/include/uapi/prt_ringbuf.h */
typedef enum {
    RBUF_UNINIT,          // Ringbuf is not inited
    RBUF_INITED           // Ringbuf is inited
} RingbufStatus;
/* From: src/include/uapi/prt_task.h */
typedef void (*TskEntryFunc)(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
/* From: src/include/uapi/prt_atomic.h */
/* struct OsSpinLock - not needed, using PrtSpinLock */
#ifndef STRUCT_PRT_SPINLOCK_DEFINED
#define STRUCT_PRT_SPINLOCK_DEFINED
typedef volatile U32 SplLock;
typedef struct PrtSpinLock {
    SplLock lock;
    uintptr_t rawLock;
} PrtSpinLock;
#endif

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

/* From: src/include/uapi/prt_task.h */
typedef U32 TskStatus;
typedef uint32_t TskPrior;
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

/* From: src/include/uapi/prt_ringbuf.h */
typedef struct {
    U32 startIdx;            // Ringbuf read index
    U32 endIdx;              // Ringbuf write index
    U32 size;                // Ringbuf total size
    U32 remain;              // Ringbuf free size
    struct PrtSpinLock lock; // Lock for read and write
    RingbufStatus status;    // Ringbuf status
    char *fifo;              // Buf to store data
} Ringbuf;
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
/* ---- Macros ---- */
/* From: src/include/uapi/prt_typedef.h */
#define ALIGN(addr, boundary) (((uintptr_t)(addr) + (boundary) - 1) & ~((uintptr_t)(boundary) - 1))
/* From: src/arch/include/prt_cpu_external.h */
#define ASM_NOP() 
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
/* From: src/include/uapi/prt_typedef.h */
#define INLINE static __inline __attribute__((always_inline))
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define NO 0
/* From: src/arch/include/prt_cpu_external.h */
#define NOP1() 
#define NOP4() 
#define NOP8() 
/* From: src/include/uapi/prt_typedef.h */
#define NULL ((void *)0)
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
/* From: src/include/uapi/prt_atomic.h */
#define OS_ERRNO_SPL_ADDR_SIZE_ILLEGAL OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x00)
#define OS_ERRNO_SPL_ADDR_SIZE_NOT_ALIGN OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x02)
#define OS_ERRNO_SPL_ALLOC_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x04)
#define OS_ERRNO_SPL_FREE_ADDR_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x06)
#define OS_ERRNO_SPL_INIT_FAIL OS_ERRNO_BUILD_FATAL(OS_MID_SPL, 0x07)
#define OS_ERRNO_SPL_NO_FREE OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x05)
#define OS_ERRNO_SPL_SIZE_INVALID OS_ERRNO_BUILD_ERROR(OS_MID_SPL, 0x03)
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
/* From: src/arch/include/prt_cpu_external.h */
#define OS_GOTO_SYS_ERROR() 
#define OS_GOTO_SYS_ERROR1() OS_GOTO_SYS_ERROR()
/* From: src/include/uapi/prt_task.h */
#define OS_HWI_HANDLE ((1U << OS_TSK_TCB_INDEX_BITS) - 1)
/* From: src/include/uapi/prt_typedef.h */
#define OS_INVALID (-1)
/* From: src/include/uapi/prt_atomic.h */
#define OS_INVALID_LOCK_OWNER_ID 0xFFFFFFFF
#define OS_LOCK_OWNER_SYS 0xFFFFFFFF
/* From: src/include/uapi/prt_typedef.h */
#define OS_OK 0
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
/* From: src/arch/include/prt_cpu_external.h */
#define PRT_CPU_EXTERNAL_H
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/include/uapi/prt_hook.h */
#define PRT_HOOK_H
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/include/uapi/prt_ringbuf.h */
#define PRT_RINGBUF_H
/* From: src/include/uapi/prt_task.h */
#define PRT_TASK_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
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
typedef uint32_t SemHandle;
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
#ifndef RINGBUF_STUBS_DISABLED
/* Ring buffer shims */
static inline U32 PRT_RingbufUsedSize(Ringbuf *rb)
{
    return (rb->size - rb->remain);
}
/* PRT_RingbufInit now from func_under_test.c */
static inline U32 PRT_RingbufRead(Ringbuf *rb, char *dest, U32 size)
{
    if (rb == NULL || dest == NULL || size == 0) return 0;
    U32 available = PRT_RingbufUsedSize(rb);
    U32 toRead = (size <= available) ? size : available;
    for (U32 i = 0; i < toRead; ++i) {
        dest[i] = rb->fifo[rb->startIdx];
        rb->startIdx = (rb->startIdx + 1) % rb->size;
    }
    rb->remain += toRead;
    return toRead;
}
static inline U32 PRT_RingbufWrite(Ringbuf *rb, const char *src, U32 size)
{
    if (rb == NULL || src == NULL || size == 0) return 0;
    U32 toWrite = (size <= rb->remain) ? size : rb->remain;
    for (U32 i = 0; i < toWrite; ++i) {
        rb->fifo[rb->endIdx] = src[i];
        rb->endIdx = (rb->endIdx + 1) % rb->size;
    }
    rb->remain -= toWrite;
    return toWrite;
}
#endif /* RINGBUF_STUBS_DISABLED */
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
extern volatile int g_hwi_lock_count;
#endif /* AUTO_STUB_PRT_PERF_RINGBUF_H */
