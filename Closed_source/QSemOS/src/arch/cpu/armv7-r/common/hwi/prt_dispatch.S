/*
 * Copyright (c) 2024, Greater Bay Area National Center of Technology Innovation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:  任务切换接口及中断处理函数实现
 * Date           Author       Notes
 * 2025-06-13     WuPeifeng    the first version
 */

#include <prt_buildef.h>
#include <prt_asm_arm_external.h>

.extern ulPortYieldRequired
.extern OsMainSchedule
.extern __svc_stack_top

FUNCTION(PRT_HwiLock)
    MRS     R0, CPSR               // 将CPSR的当前值存入R0
    CPSID   I                      // 禁用中断（将CPSR中的I位设为1）
    BX      LR                     // 返回

FUNCTION(PRT_HwiUnLock)
    MRS     R0, CPSR               // 将CPSR的当前值存入R0
    CPSIE   I                      // 使能中断（将CPSR中的I位设为0）
    BX      LR                     // 返回

FUNCTION(PRT_HwiRestore)
    MSR     cpsr_c, r0
    BX      LR


FUNCTION(OsTaskTrap)
    LDR    r1, =g_runningTask /* OsTaskTrap是函数调用过来，r0 r1寄存器是caller save，此处能直接使用 */
    LDR    r0, [r1] /* r0 is the &g_pRunningTask->sp */
/*
 * 描述: Task调度处理函数。 SMP调度走这个接口，X0 is SP
 */
OsTskTrapSmp:
    MRS     r1, SPSR
    PUSH    {r1}
    PUSH    {lr}
    PUSH    {r0-r12, lr}

    // 存入SP指针到g_pRunningTask->sp
    mov    r1, sp
    str    r1, [r0]   // x0 is the &g_pRunningTask->sp

    ldr    r0, =__exc_stack_top
    ldr    r0, [r0]

    mov    sp, r0
    B      OsMainSchedule
loop1:
    B      loop1

FUNCTION(OsTskContextLoad)
    /* clear the flag of ldrex */
    CLREX
    /* switch to new task's sp */
    LDR     SP, [R0]

    POP     {R0-R12, LR}

    POP     {R0}
    POP     {R1}
    MSR     CPSR, R1
    BX      R0

FUNCTION(OsHwiDispatcher)
    SUB     LR, LR, #4

    /* save spsr and lr(svc's pc) onto the svc stack */
    SRSDB   #0x13!

    /* disable irq, switch to svc mode */
    CPSID   i, #0x13

    /* push caller saved regs as trashed regs */
    PUSH    {R0-R3, R12, LR}

    /* 8 bytes stack align
    STACK_ALIGN     R0 */

    MOV     R0, sp
    TST     SP, #4
    SUBEQ   SP, #4
    PUSH    {R0}

    PUSH    {R4}
    MOV     R4, SP

    ldr     R1, =__svc_stack_top
    ldr     R1, [R1]
    mov     sp, R1                  /* set  sp */

    BLX     OsHwiDispatchHandle

    DSB
    ISB
    MOV     SP, R4
    POP     {R4}

    // ulPortYieldRequired 不为0时，表示需要任务切换
    LDR		r1, =ulPortYieldRequired
	LDR		r0, [r1]
	CMP		r0, #0
    BLNE    OsTaskTrap

    POP     {R0}
    MOV     sp, R0

OsIrqContextRestore:
    POP     {R0-R3, R12, LR}
    RFEIA   SP!

