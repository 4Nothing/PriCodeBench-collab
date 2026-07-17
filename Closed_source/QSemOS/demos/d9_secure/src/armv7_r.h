/*
 * Copyright (c) 2024, Greater Bay Area National Center of Technology Innovation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:  任务切换接口及中断处理函数实现
 * Date           Author       Notes
 * 2025-06-13     WuPeifeng    the first version：armv7-a/r定义
 */

#ifndef ARMV7_R_H
#define ARMV7_R_H

#include <prt_buildef.h>

/* PSR */
#define PSR_USR_MODE           0x00000010u
#define PSR_FIQ_MODE           0x00000011u
#define PSR_IRQ_MODE           0x00000012u
#define PSR_SVC_MODE           0x00000013u
#define PSR_ABT_MODE           0x00000017u
#define PSR_UNDEF_MODE         0x0000001Bu
#define PSR_HYP_MODE           0x0000001Au
#define PSR_SYS_MODE           0x0000001Fu
#define PSR_MODE_MASK          0x0000001Fu

#define PSR_F_BIT               0x00000040u
#define PSR_I_BIT               0x00000080u
#define PSR_A_BIT               0x00000100u

#define PSR_T_ARM           0x00000000u
#define PSR_T_THUMB         0x00000020u

#define PSR_INT_DISABLE     (PSR_I_BIT | PSR_F_BIT)

/* MPIDR field defines */
#define MPIDR_CPUID_MASK    0x000000FFu

#define ICACHE              2               //The old value is 1
#define DCACHE              1               //The old value is 2
#define UCACHE              (ICACHE|DCACHE)

#define __ALIGNED(x)        __attribute__((aligned(x)))
#define __CACHE_ALIGN       __ALIGNED(OS_CACHE_LINE_SIZE)


#endif
