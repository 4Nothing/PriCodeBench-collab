/*
 * Copyright (c) 2024, Greater Bay Area National Center of Technology Innovation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:  任务切换接口及中断处理函数实现
 * Date           Author       Notes
 * 2025-06-14     WuPeifeng    the first version：armv7-a/r定义
 */

#include "prt_sys.h"
#include "prt_tick.h"
#include "prt_config.h"
#include "prt_task.h"
#include "prt_hwi.h"
#include "cpu_config.h"
#include "securec.h"
#include <stdio.h>

#define TIMER3_BASE_REG         0xF08B0000
#define TIMER3_IRQ_NUM          229

#define REG_TMR_STA             0x00
#define REG_TMR_STA_EN          0x4
#define REG_TMR_SIG_EN          0x8
#define REG_TMR_CLK_CFG         0x20
#define REG_TMR_CNT_CFG         0x24
#define REG_CNT_G0_OVF          0x28
#define REG_CNT_G1_OVF          0x2C
#define REG_LOCAL_A_OVF         0x30
#define REG_CNT_G0              0x40
#define REG_CNT_G1              0x44
#

uint64_t PRT_ClkGetCycleCount64(void)
{
    U32 countLow, countTmp, countHigh;

    countLow = REG_READ(TIMER3_BASE_REG + REG_CNT_G0);
    do {
        countTmp = countLow;
        countHigh = REG_READ(TIMER3_BASE_REG + REG_CNT_G1);
        countLow = REG_READ(TIMER3_BASE_REG + REG_CNT_G0);
    } while (countTmp > countLow);

    return (((uint64_t)countHigh) << 32) + countLow;
}

static void TimerIsr(uintptr_t para)
{
    // 重置一个tick中断
    U32 status = REG_READ(TIMER3_BASE_REG + REG_TMR_STA);
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_STA, status);

    PRT_TickISR();
    PRT_ISB();
}

static uint32_t CoreTimerStart(void)
{
    U32 reg_value;

    // 设置LOCAL_A计时器超时值为一个tick周期的cycle值-1
    REG_WRITE(TIMER3_BASE_REG + REG_LOCAL_A_OVF, ((OS_SYS_CLOCK / OS_TICK_PER_SECOND)- 1));

    reg_value = REG_READ(TIMER3_BASE_REG + REG_TMR_CNT_CFG);
    reg_value |= (0x1 << 2); // reload CNT_LOCAL_A
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_CNT_CFG, reg_value);

    // sdrv_tmr_lld_int_enable(TIMER3_BASE_REG, SDRV_TMR_STA_CNT_OVF_SHIFT(id));
    reg_value = REG_READ(TIMER3_BASE_REG + REG_TMR_STA_EN);
    reg_value |= (0x1 << 10);
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_STA_EN, reg_value);

    reg_value = REG_READ(TIMER3_BASE_REG + REG_TMR_SIG_EN);
    reg_value |= (0x1 << 10);
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_SIG_EN, reg_value);
    return OS_OK;
}


uint32_t TestClkStart(void)
{
    uint32_t ret;
    uint32_t reg_value;

    /* 初始化寄存器 */
    reg_value = REG_READ(TIMER3_BASE_REG + REG_TMR_CLK_CFG);
    reg_value &= 0xfffc0000;
    /* 设置时钟24Mhz，不分频*/
    reg_value |= (0x2 << 16);
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_CLK_CFG, reg_value);

    // 设置 CASCADE_MODE
    reg_value = REG_READ(TIMER3_BASE_REG + REG_TMR_CNT_CFG);
    reg_value |= (1 << 6);
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_CNT_CFG, reg_value);

    /* 设置0xffffffff，G0和G1一起实现64位count计时 */
    REG_WRITE(TIMER3_BASE_REG + REG_CNT_G0_OVF, 0xffffffff);
    REG_WRITE(TIMER3_BASE_REG + REG_CNT_G1_OVF, 0xffffffff);
    reg_value = REG_READ(TIMER3_BASE_REG + REG_TMR_CNT_CFG);
    reg_value |= (0x1 << 0); // reload CNT_G0
    reg_value |= (0x1 << 1); // reload CNT_G1
    REG_WRITE(TIMER3_BASE_REG + REG_TMR_CNT_CFG, reg_value);

    ret = PRT_HwiSetAttr(TIMER3_IRQ_NUM, 2, OS_HWI_MODE_ENGROSS);
    if (ret != OS_OK) {
        return ret;
    }

    ret = PRT_HwiCreate(TIMER3_IRQ_NUM, (HwiProcFunc)TimerIsr, 0);
    if (ret != OS_OK) {
        return ret;
    }

    PRT_HwiEnable(TIMER3_IRQ_NUM);

    ret = CoreTimerStart();
    if (ret != OS_OK) {
        return ret;
    }

    return OS_OK;
}


