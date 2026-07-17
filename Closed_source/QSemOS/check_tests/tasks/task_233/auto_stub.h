/*
 * Auto-generated stub header for prt_dynmodule.c
 * Generated from QSemOS real headers — all definitions are verbatim copies.
 * Source file: src/core/extend/dlmodule/prt_dynmodule.c
 * Included headers: 11
 */
#ifndef AUTO_STUB_PRT_DYNMODULE_H
#define AUTO_STUB_PRT_DYNMODULE_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <elf.h>

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
typedef signed char S8;
typedef signed short S16;
typedef signed long long S64;
typedef int S32;
typedef void *VirtAddr;
typedef void *PhyAddr;

/* ELF type definitions forDynModule (use 64-bit types for x86_64) */
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Rel Elf_Rel;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Sword Elf_Sword;
typedef Elf64_Word Elf_Word;

/* ---- Structural macros (used in type definitions) ---- */
/* From: src/include/uapi/prt_task.h */
#define OS_TSK_NAME_LEN 16
/* ---- Types ---- */
/* From: src/core/extend/dlmodule/prt_dynmodule_internal.h */
typedef U32 (*OsDynModuleRelocateFunc)(Elf_Addr relocAddr, struct OsDynModuleRelocInfo relocInfo);
/* From: src/include/uapi/prt_task.h */
typedef U32 TskHandle;
typedef U16 TskPrior;
typedef U16 TskStatus;
typedef void (*TskEntryFunc)(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
/* From: src/include/uapi/prt_typedef.h */
typedef void (*OsVoidFunc)(void);
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
/* From:  */
enum OS_MODULE_ERRNO_E {
    OS_MODULE_OK = 0,                             // 成功
    OS_MODULE_ERRNO_ELF_HEAD_LEN,                 // ELF头长度不正确
    OS_MODULE_ERRNO_ELF_HEAD_MAGIC,               // ELF头魔数字不正确
    OS_MODULE_ERRNO_ELF_HEAD_CLASS,               // ELF头class不正确
    OS_MODULE_ERRNO_ELF_HEAD_SHENT_SIZE,          // ELF头section header table size不正确
    OS_MODULE_ERRNO_ELF_HEAD_SHNUM,               // ELF头section header table数量不正确
    OS_MODULE_ERRNO_ELF_HEAD_MACHINE,             // ELF头machine不正确
    OS_MODULE_ERRNO_ELF_HEAD_TYPE,                // ELF头type不正确
    OS_MODULE_ERRNO_UNIT_FULL,                    // 模块单元已满
    OS_MODULE_ERRNO_PTLOAD_SIZE,                  // PT_LOAD段大小不正确
    OS_MODULE_ERRNO_MEMORY_ALLOC,                 // 内存分配失败
    OS_MODULE_ERRNO_MEMCOPY,                      // 内存拷贝失败
    OS_MODULE_ERRNO_OS_SYM,                       // 获取OS的符号失败
    OS_MODULE_ERRNO_RELOCATE_INVALID_TYPE,        // 重定位类型错误
    OS_MODULE_ERRNO_RELOCATE,                     // 重定位失败
    OS_MODULE_ERRNO_DYNSYM_SECTION_NOT_FOUND,     // 找不到.dynsym段
    OS_MODULE_ERRNO_RELOCATE_SYM_VAL_OVERFLOW,    // 重定位符号值溢出
    OS_MODULE_ERRNO_NO_GLOBAL_FUNC,               // 没有全局函数
    OS_MODULE_ERRNO_FILE_CHECK_FAILED,            // 模块文件校验失败
    OS_MODULE_ERRNO_FILE_OPEN,                    // 模块文件打开失败
    OS_MODULE_ERRNO_FILE_READ,                    // 模块文件读取失败
};
enum ModuleUnitSate {
    MODULE_UNIT_FREE = 0,
    MODULE_UNIT_INIT,
    MODULE_UNIT_ACTIVE,
};

/* DynModule symbol table entry */
typedef struct DynModuleSymTab {
    void *addr;
    char *name;
} DynModuleSymTab;

/* DynModule unit info - must match prt_dynmodule_internal.h */
struct DynModuleUnitInfo {
    U8 unitNo;
    U16 symNum;
    enum ModuleUnitSate state;
    U64 loadSegStartAddr;
    U64 loadSegEndAddr;
    dev_t stDev;
    ino_t stIno;
    struct DynModuleSymTab *symTab;
    uint8_t *loadSegMem;
    char *moduleStr;
    char *error;
};

/* Relocation info structure */
struct OsDynModuleRelocInfo {
    Elf_Word relocType;
    Elf_Word shType;
    Elf_Addr symAddr;
    uintptr_t reloc;
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
/* From: src/core/extend/dlmodule/prt_dynmodule_internal.h */
struct OsDynModuleRelocateMap {
    U32 relType;
    OsDynModuleRelocateFunc relFunc;
};
/* From: src/include/uapi/prt_task.h */
typedef U32(*TskSwitchHook)(TskHandle lastTaskPid, TskHandle nextTaskPid);
typedef U32(*TskDeleteHook)(TskHandle taskPid);
typedef U32(*TskCreateHook)(TskHandle taskPid);
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
/* From: src/include/uapi/prt_hook.h */
struct HookModInfo {
    U8 maxNum[(U32)OS_HOOK_TYPE_NUM];
};
/* From: src/include/uapi/prt_task.h */
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
/* ---- Macros ---- */
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
/* From: src/core/extend/dlmodule/prt_dynmodule_internal.h */
#define OS_ELF32_R_SYM(info) ((info) >> 8)            /* r_info的高24位表示重定位入口的符号在符号表中的下标 */
#define OS_ELF32_R_TYPE(info) ((info) & 0xff)
#define OS_ELF64_R_SYM(info) ((info) >> 32)           /* r_info的高32位表示重定位入口的符号在符号表中的下标 */
#define OS_ELF64_R_TYPE(info) ((info) & 0xffffffff)
/* From: src/include/uapi/prt_typedef.h */
#define OS_EMBED_ASM __asm__ __volatile__
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
/* From: src/include/uapi/prt_mem.h */
#define OS_MEM_ADDR_ALIGN sizeof(uintptr_t)
#define OS_MEM_DEFAULT_PT0 0
#define OS_MEM_DEFAULT_PT1 1
#define OS_MEM_DEFAULT_PTNUM 2
#define OS_MEM_DEFAULT_FSC_PT OS_MEM_DEFAULT_PT0

/* DynModule specific macros */
#define OS_MAX_MODULE_NUM 8
#define OS_MODULE_ALIGN_LEN 4096
#define OS_MODULE_ERROR_STR_LEN 512
#define MEM_ADDR_ALIGN_4K 4096

/* Memory alignment helper */
#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

/* From: src/include/uapi/prt_typedef.h */
#define OS_OK 0
/* From: src/core/extend/include/prt_dynamic_module.h */
#define OS_SECTION(info) __attribute__((section(info)))
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
/* From: src/core/extend/include/prt_dynamic_module.h */
#define OS_SYMBOL_EXPORT(symbol) 
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
/* From: src/arch/include/prt_attr_external.h */
#define PRT_ATTR_EXTERNAL_H
/* From: src/arch/include/prt_cpu_external.h */
#define PRT_CPU_EXTERNAL_H
/* From: src/core/extend/dlmodule/prt_dynmodule_internal.h */
#define PRT_DYNMODULE_INTERNAL_H
/* From: src/include/uapi/prt_errno.h */
#define PRT_ERRNO_H
/* From: src/include/uapi/prt_hook.h */
#define PRT_HOOK_H
/* From: src/core/extend/include/prt_dynamic_module.h */
#define PRT_LP_H
/* From: src/include/uapi/prt_mem.h */
#define PRT_MEM_H
/* From: src/include/uapi/prt_module.h */
#define PRT_MODULE_H
/* From: src/include/uapi/prt_task.h */
#define PRT_TASK_H
/* From: src/include/uapi/prt_typedef.h */
#define PRT_TYPEDEF_H
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX-1)
/* From: platform/libboundscheck/include/securec.h */
#define SECUREC_H
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
static inline void *PRT_MemAllocAlign(U32 pool, U32 pt, U32 size, U32 align)
{
    (void)pool; (void)pt;
    void *p = NULL;
    if (posix_memalign(&p, align, size) != 0)
        return NULL;
    return p;
}
/* Module interrupt lock/unlock */
static inline uintptr_t OsModuleIntLock(void) { return 0; }
static inline void OsModuleIntUnlock(uintptr_t intSave) { (void)intSave; }

/* EOK for securec functions */
#define EOK 0

/* ELF macros */
#define ELF_R_SYM(info) ((info) >> 8)
#define ELF_R_TYPE(info) ((info) & 0xff)
#define ELF_ST_BIND(info) ((info) >> 4)
#define ELF_ST_TYPE(info) ((info) & 0xf)
#define ELF_ST_INFO(bind, type) (((bind) << 4) | ((type) & 0xf))
#define ELF_R_INFO(sym, type) ((((Elf64_Word)(sym)) << 32) | (type))

/* SecureC function shims */
typedef int errno_t;
#define strcpy_s(dst, dstsz, src) strcpy(dst, src)
#define memcpy_s(dst, dstsz, src, count) memcpy(dst, src, count)
#define vsnprintf_s(buf, bufsz, count, fmt, ap) vsnprintf(buf, bufsz, fmt, ap)

/* Missing function declarations */
static inline uintptr_t OsDynModuleFindSymFromOs(const char *symName)
{
    (void)symName;
    return 0;
}
static inline U32 OsDynModuleRelocate(Elf_Addr relocAddr, struct OsDynModuleRelocInfo relocInfo)
{
    (void)relocAddr;
    (void)relocInfo;
    return OS_OK;
}

/* Architecture-specific stubs */
static inline void os_asm_invalidate_dcache_all(void) { }
static inline void os_asm_invalidate_icache_all(void) { }

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
#endif /* AUTO_STUB_PRT_DYNMODULE_H */
