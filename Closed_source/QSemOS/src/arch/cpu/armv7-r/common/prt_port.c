/*
 * Copyright (c) 2024, Greater Bay Area National Center of Technology Innovation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:  任务切换接口及中断处理函数实现
 * Date           Author       Notes
 * 2025-06-13     WuPeifeng    the first version：各模式下栈初始化及任务栈初始化
 */

#include "prt_typedef.h"
#include "prt_cpu_external.h"
#include "prt_sys_external.h"

#ifdef __thumb__
#define ARMV7R_SPSR_INIT_VALUE      (PSR_SVC_MODE | PSR_T_THUMB | PSR_F_BIT)
#else
#define ARMV7R_SPSR_INIT_VALUE        (PSR_SVC_MODE | PSR_T_ARM   | PSR_F_BIT)
#endif

/* Tick中断对应的硬件定时器ID */
// OS_SEC_DATA U32 g_tickTimerID = U32_INVALID;
// 系统栈配置
uint8_t abt_stack[OS_ARCH_ABT_STACK_SIZE] __CACHE_ALIGN OS_SEC_DATA;
uint8_t sys_stack[OS_ARCH_SYS_STACK_SIZE] __CACHE_ALIGN OS_SEC_DATA;
uint8_t und_stack[OS_ARCH_UND_STACK_SIZE] __CACHE_ALIGN OS_SEC_DATA;
uint8_t fiq_stack[OS_ARCH_FIQ_STACK_SIZE] __CACHE_ALIGN OS_SEC_DATA;
uint8_t irq_stack[OS_ARCH_IRQ_STACK_SIZE] __CACHE_ALIGN OS_SEC_DATA;
uint8_t svc_stack[OS_ARCH_SVC_STACK_SIZE] __CACHE_ALIGN OS_SEC_DATA;

uintptr_t __exc_stack_top = (uintptr_t)(sys_stack + OS_ARCH_SYS_STACK_SIZE);
uintptr_t __svc_stack_top = (uintptr_t)(svc_stack + OS_ARCH_SVC_STACK_SIZE);

/*
 * 描述: 分配核的系统栈空间
 */
// INIT_SEC_L4_TEXT void InitSystemSp(uintptr_t sysStackHigh, uintptr_t svcStackHigh)
// {
//     return;
// }

/*
 * 描述: 获取系统栈的起始地址（低地址)
 */
INIT_SEC_L4_TEXT uintptr_t OsGetSysStackStart(U32 core)
{
    (void)core;
    return (uintptr_t)sys_stack;
}

/*
 * 描述: 获取系统栈的结束地址（高地址)
 */
INIT_SEC_L4_TEXT uintptr_t OsGetSysStackEnd(U32 core)
{
    (void)core;
    return __exc_stack_top;
}

// /*
//  * 描述: 获取系统栈的栈底（高地址)
//  */
// OS_SEC_L0_TEXT uintptr_t OsGetSysStackSP(U32 core)
// {
//     return OsGetSysStackEnd(core);
// }

INIT_SEC_L4_TEXT void *OsTskContextInit(U32 taskID, U32 stackSize, uintptr_t *topStack, uintptr_t funcTskEntry)
{
    (void)taskID;
    struct TskContext *stack = (struct TskContext *)((uintptr_t)topStack + stackSize);

    stack -= 1;
    stack->reg12 = 0x12121212;
    stack->reg11 = 0x11111111;
    stack->reg10 = 0x10101010;
    stack->reg9 = 0x09090909;
    stack->reg8 = 0x08080808;
    stack->reg7 = 0x07070707;
    stack->reg6 = 0x06060606;
    stack->reg5 = 0x05050505;
    stack->reg4 = 0x04040404;
    stack->reg3 = 0x03030303;
    stack->reg2 = 0x02020202;
    stack->reg1 = 0x01010101;
    stack->reg0 = 0x00000000;
    stack->lr = funcTskEntry;
    stack->spsr = ARMV7R_SPSR_INIT_VALUE;
    stack->pc = funcTskEntry;

    return stack;
}

/*
 * 描述: 从指定地址获取任务上下文
 */
OS_SEC_L4_TEXT void OsTskContextGet(uintptr_t saveAddr, struct TskContext *context)
{
    *context = *((struct TskContext *)saveAddr);

    return;
}

/*
 * 描述: 手动触发异常（EL1）
 */
OS_SEC_L4_TEXT void OsAsmIll(void)
{
    OS_EMBED_ASM("svc  0");
}
