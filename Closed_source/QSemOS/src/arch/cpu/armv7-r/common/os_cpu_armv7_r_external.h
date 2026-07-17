/*
 * Copyright (c) 2009-2022 Huawei Technologies Co., Ltd. All rights reserved.
 *
 * UniProton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Create: 2024-11-04
 * Description: 属性宏相关内部头文件
 */
#ifndef OS_CPU_ARMV7_R_EXTERNAL_H
#define OS_CPU_ARMV7_R_EXTERNAL_H

#include "prt_buildef.h"
#include "prt_hwi.h"
#include "prt_gic_external.h"
#include "prt_atomic.h"
#include "prt_clk.h"
#include "armv7_r.h"

/*
 * 模块间宏定义
 */
#define OS_IRQ2HWI(irqNum)           (irqNum)
#define OS_HWI2IRQ(hwiNum)           (hwiNum)
#define OS_HWI_GET_HWINUM(archNum)   (archNum)
#define OS_HWI_GET_HWI_PRIO(hwiPrio) (hwiPrio)
#define OS_HWI_IS_SGI(hwiNum)        ((hwiNum) <= MAX_SGI_ID)
#define OS_HWI_IS_PPI(hwiNum)        (((hwiNum) > MAX_SGI_ID) && ((hwiNum) <= MAX_PPI_ID))

/* OS_HWI_MAX_NUM 大小会影响bss段大小。需要根据实际使用hwi个数配置 */
#if defined(OS_OPTION_GIC_LPI)
#define OS_HWI_MAX_NUM           0x10000U  /* 后续整改这里，当前这样用 */
#define OS_HWI_NUM_CHECK(hwiNum)    ((hwiNum) >= OS_HWI_MAX_NUM) || \
    (((hwiNum) > MAX_SPI_ID) && ((hwiNum) < MIN_LPI_ID))
#else
#define OS_HWI_MAX_NUM           0x120U
#define OS_HWI_NUM_CHECK(hwiNum) ((hwiNum) >= OS_HWI_MAX_NUM)
#endif

#define OS_HWI_MAX           (OS_HWI_MAX_NUM - 1)
#define OS_HWI_FORMARRAY_NUM OS_HWI_MAX_NUM
#define OS_HWI_MIN           0
#define OS_HWI_PRI_NUM       0x10

/* 中断优先级0~15，但非安全世界的中断优先级只能是偶数 */
#define OS_HWI_PRIO_CHECK(hwiPrio)     ((hwiPrio) >= OS_HWI_PRI_NUM || ((hwiPrio) & 1U))
#define OS_HWI_SET_HOOK_ATTR(hwiNum, hwiPrio, hook)

#if defined(OS_OPTION_HWI_AFFINITY)
/* 仅1-1/1-N SPI中断支持中断路由 */
#define OS_HWI_AFFINITY_CHECK(hwiNum) OsGicIsSpi(hwiNum)

#endif

#define OS_HWI_CLEAR_CHECK(hwiNum)    ((hwiNum) == GIC_INT_ID_MASK)

/*
 * SMP系统占用的硬件SGI号
 */
#define OS_SMP_SCHED_TRIGGER_OTHER_CORE_SGI OS_HWI_IPI_NO_01 // 触发它核响应一次调度的IPI中断号
#define OS_SMP_EXC_STOP_OTHER_CORE_SGI      OS_HWI_IPI_NO_02 // 一个核异常后将其他核停住的IPI中断号
#define OS_SMP_TICK_TRIGGER_OTHER_CORE_SGI  OS_HWI_IPI_NO_03 // 响应tick中断的核触发它核的模拟tickIPI中断号
#define OS_SMP_MC_CORE_IPC_SGI              OS_HWI_IPI_NO_04 // SMP核间通信使用的IPI中断号

/*
 * SMP系统占用的硬件SGI的优先级
 */
#define OS_SMP_SCHED_TRIGGER_OTHER_CORE_SGI_PRI 0
#define OS_SMP_EXC_STOP_OTHER_CORE_SGI_PRI      0  // 一个核异常后将其他核停住的IPI中断号
#define OS_SMP_TICK_TRIGGER_OTHER_CORE_SGI_PRI  0
#define OS_SMP_MC_CORE_IPC_SGI_PRI              0

#define OS_HWI_INTERNAL_NUM 5

#define OS_DI_STATE_CHECK(intSave)   ((intSave) & 0x80U)

#define OS_TICK_COUNT_UPDATE()
OS_SEC_ALW_INLINE INLINE void OsSpinLockInitInner(volatile uintptr_t *lockVar)
{
    *lockVar = OS_SPINLOCK_UNLOCK;
}

#define OS_SPINLOCK_INIT_FOREACH(maxNum, structName, field)
#define OS_SPIN_FREE_FOREACH(maxNum, structName, field)
#define OS_SPIN_FREE(lockVar)

#define OS_HW_TICK_INIT() OS_OK

#define OS_IS_TICK_PERIOD_INVALID(cyclePerTick) (FALSE)

#define OS_TSK_STACK_SIZE_ALIGN  16U
#define OS_TSK_STACK_SIZE_ALLOC_ALIGN MEM_ADDR_ALIGN_016
#define OS_TSK_STACK_ADDR_ALIGN  16U
#define OS_ALLCORES_MASK g_validAllCoreMask

#define OS_MAX_CACHE_LINE_SIZE   4 /* 单核芯片定义为4 */

/* 任务栈最小值 */
#define OS_TSK_MIN_STACK_SIZE (ALIGN((0x1D0 + 0x10 + 0x4), 16))
/* Idle任务的消息队列数 */
#define OS_IDLE_TASK_QUE_NUM 1

/* Unwind 相关 寄存器在解析帧中的位置，前面30个通用寄存器忽略 */
#define OS_UNWEIND_LR_OFFSET 0x1EU
#define OS_UNWEIND_SP_OFFSET 0x24U
#define OS_UNWEIND_PC_OFFSET 0x23U

/* SPINLOCK 相关 */
#define OS_SPINLOCK_RELOCK_NEGATIVE 0x80000000U

extern U64 OsU64DivGetQuotient(U64 dividend, U64 divisor);
extern U64 OsU64DivGetRemainder(U64 dividend, U64 divisor);
#define DIV64(a, b) OsU64DivGetQuotient((a), (b))
#define DIV64_REMAIN(a, b) OsU64DivGetRemainder((a), (b))

#define OsIntUnLock() PRT_HwiUnLock()
#define OsIntLock()   PRT_HwiLock()
#define OsIntRestore(intSave) PRT_HwiRestore(intSave)

/*
 * 任务上下文的结构体定义。
 */
struct TagHwContext {
    uintptr_t reg12;
    uintptr_t reg11;
    uintptr_t reg10;
    uintptr_t reg9;
    uintptr_t reg8;
    uintptr_t reg7;
    uintptr_t reg6;
    uintptr_t reg5;
    uintptr_t reg4;
    uintptr_t reg3;
    uintptr_t reg2;
    uintptr_t reg1;
    uintptr_t reg0;
    uintptr_t lr;
    uintptr_t pc;
    uintptr_t spsr;
};

/* StackTrace保存的任务上下文 */
struct TagStackTraceContext {
    uintptr_t lr;
    uintptr_t spsr;
    uintptr_t sp;
    uintptr_t pc;
    uintptr_t rregs[13];     // r0~r12
};

/*
 * 模块间变量声明
 */
extern OsVoidFunc g_hwiSplLockHook;
extern OsVoidFunc g_hwiSplUnLockHook;
extern U32 g_validAllCoreMask;
extern U32 ulPortYieldRequired;
/*
 * 模块间函数声明
 */
extern void OsTaskTrap(void);
extern void OsTskContextLoad(uintptr_t stackPointer);
extern uintptr_t OsGetSysStackStart(U32 core);
extern uintptr_t OsGetSysStackEnd(U32 core);
/*
 * 描述: spinlock初始化
 */
OS_SEC_ALW_INLINE INLINE U32 OsSplLockInit(struct PrtSpinLock *spinLock)
{
    OsSpinLockInitInner(&spinLock->rawLock);
    return OS_OK;
}

/*
 * 描述: 获取硬线程ID
 */
OS_SEC_ALW_INLINE INLINE U32 OsGetHwThreadId(void)
{
    return PRT_GetCoreID();
}

/* 计算一个32bit非0数字的最右位     */
/* e.g. 0x01000020 ----> 结果返回 5 */
OS_SEC_ALW_INLINE INLINE U32 OsGetRMB(U32 bit)
{
    U32 rev = bit - 1;
    U32 result;

    OS_EMBED_ASM("EOR %0, %1, %2" : "=r"(result) : "r"(rev), "r"(bit));
    OS_EMBED_ASM("CLZ %0, %1" : "=r"(rev) : "r"(result));
    return (OS_DWORD_BIT_NUM - rev -1);
}

/* 计算一个32bit非0 bit的最左位数   */
OS_SEC_ALW_INLINE INLINE U32 OsGetLMB1(U32 value)
{
    U32 mb;

    OS_EMBED_ASM("CLZ %w0, %w1" : "=r"(mb) : "r"(value));

    return mb;
}

#define OsIntEnable()  PRT_HwiUnLock()
#define OsIntDisable()  PRT_HwiLock()

/*
 * 描述: 使能FIQ中断
 */
OS_SEC_ALW_INLINE INLINE void OsFiqEnable(void)
{
    OS_EMBED_ASM("CPSIE F");
}

/*
 * 描述: 设置中断亲核性
 */
OS_SEC_ALW_INLINE INLINE U32 OsHwiAffinitySet(HwiHandle hwiNum, U32 coreMask)
{
    U32 targetCore;

    if ((coreMask & (coreMask - 1)) != 0) {
        return OS_ERRNO_MULTI_TARGET_CORE;
    }

    targetCore = OsGetRMB(coreMask);
    OsGicSetTargetId(hwiNum, targetCore);

    return OS_OK;
}

OS_SEC_ALW_INLINE INLINE U32 OsHwiNumGet(void)
{
    U32 iar;

    iar = GIC_REG_READ(GICC_IAR);

    return (iar & IAR_MASK);
}

OS_SEC_ALW_INLINE INLINE void OsHwiClear(U32 intId)
{
    GIC_REG_WRITE(GICC_EOIR, intId & IAR_MASK);
}

OS_SEC_ALW_INLINE INLINE uintptr_t PRT_GetCpsr(void)
{
    U32 cpsr;

    __asm__ volatile
    (
        "\tmrs    %0, cpsr\n"
        "\tcpsid  i\n"
        : "=r" (cpsr)
        :
        : "memory"
    );

    return cpsr;
}

/*
 * 描述: 获取SP
 */
OS_SEC_ALW_INLINE INLINE uintptr_t OsGetSp(void)
{
    uintptr_t sp;

    OS_EMBED_ASM("MOV  %0, sp" : "=r"(sp));

    return sp;
}

/*
 * 描述: 获取LR
 */
OS_SEC_ALW_INLINE INLINE uintptr_t OsGetLR(void)
{
    uintptr_t LR_Get;

    OS_EMBED_ASM("MOV  %0, lr" : "=r"(LR_Get));

    return LR_Get;
}

/*
 * 描述: 获取PC
 */
OS_SEC_ALW_INLINE INLINE uintptr_t OsGetPC(void)
{
    uintptr_t pc;

    OS_EMBED_ASM("1:adr  %0, pc" : "=r"(pc));

    return pc;
}

/*
 * 描述: 传入任务切换时的栈地址
 */
OS_SEC_ALW_INLINE INLINE uintptr_t OsTskGetInstrAddr(uintptr_t addr)
{
    return ((struct TagHwContext *)addr)->pc;
}

OS_SEC_ALW_INLINE INLINE void OsTaskTrapFast(void)
{
    OsTaskTrap();
}

OS_SEC_ALW_INLINE INLINE void OsTaskTrapFastPs(uintptr_t intSave)
{
    (void)intSave;
    OsTaskTrap();
}

OS_SEC_ALW_INLINE INLINE void OsSplLock(volatile uintptr_t *spinLock)
{
    (void)spinLock;
}

OS_SEC_ALW_INLINE INLINE void OsSplUnlock(volatile uintptr_t *spinLock)
{
    (void)spinLock;
}

OS_SEC_ALW_INLINE INLINE void OsSplReadLock(volatile uintptr_t *spinLock)
{
    (void)spinLock;
}

OS_SEC_ALW_INLINE INLINE void OsSplReadUnlock(volatile uintptr_t *spinLock)
{
    (void)spinLock;
}

OS_SEC_ALW_INLINE INLINE void OsSplWriteLock(volatile uintptr_t *spinLock)
{
    (void)spinLock;
}

OS_SEC_ALW_INLINE INLINE void OsSplWriteUnlock(volatile uintptr_t *spinLock)
{
    (void)spinLock;
}

OS_SEC_ALW_INLINE INLINE void OsHwiSetSplLockHook(OsVoidFunc hook)
{
    (void)hook;
}

OS_SEC_ALW_INLINE INLINE void OsHwiSetSplUnlockHook(OsVoidFunc hook)
{
    (void)hook;
}
#endif /* OS_CPU_ARMV7_R_EXTERNAL_H */
