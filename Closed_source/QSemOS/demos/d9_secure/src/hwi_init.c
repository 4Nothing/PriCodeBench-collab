/*
 * Copyright (c) 2024, Greater Bay Area National Center of Technology Innovation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:  任务切换接口及中断处理函数实现
 * Date           Author       Notes
 * 2025-06-13     WuPeifeng    the first version：armv7-a/r定义
 */

#include <prt_hwi.h>
#include <cpu_config.h>

#define MAX_SPI_ID 274

void OsGicInitCpuInterface(void)
{
    int i;
    U32 val;

    GIC_REG_WRITE(GICD_CTLR, 1);
    /* disable int */
    for (i = 0; i < MAX_SPI_ID; i += 32) {
        GIC_REG_WRITE(GICD_ICENABLERn + (i/8), 0xFFFFFFFF);
        GIC_REG_WRITE(GICD_ICPENDRn + (i/8), 0xFFFFFFFF);
    }

    /* set SPI cpu to cpu 0 and Non-secure view */
    for (i = 32; i < MAX_SPI_ID; i += 4) {
        GIC_REG_WRITE(GICD_IPRIORITYn + i, 0x80808080);
        GIC_REG_WRITE(GICD_ITARGETSRn + i, 0x01010101);
    }

    GIC_REG_WRITE(GICC_PMR, 0xFF);

    GIC_REG_WRITE(GICC_CTLR, 1);
}

U32 OsHwiInit(void)
{
    OsGicInitCpuInterface();
    PRT_HwiUnLock();
    return OS_OK;
}
