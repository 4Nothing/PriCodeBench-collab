/*
 * Copyright (c) 2022-2022 Huawei Technologies Co., Ltd. All rights reserved.
 *
 * UniProton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Create: 2022-11-22
 * Description: 异常模块的对外头文件。
 */
#ifndef ARMV7_R_EXC_H
#define ARMV7_R_EXC_H

#include "prt_typedef.h"
#include "prt_sys.h"
#if defined(OS_OPTION_HAVE_FPU)
#include "os_cpu_armv7_r.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

struct ExcRegInfo {
    uintptr_t regCPSR; /**< Current program status register (CPSR) */
    uintptr_t R0;      /**< Register R0 */
    uintptr_t R1;      /**< Register R1 */
    uintptr_t R2;      /**< Register R2 */
    uintptr_t R3;      /**< Register R3 */
    uintptr_t R4;      /**< Register R4 */
    uintptr_t R5;      /**< Register R5 */
    uintptr_t R6;      /**< Register R6 */
    uintptr_t R7;      /**< Register R7 */
    uintptr_t R8;      /**< Register R8 */
    uintptr_t R9;      /**< Register R9 */
    uintptr_t R10;     /**< Register R10 */
    uintptr_t R11;     /**< Register R11 */
    uintptr_t R12;     /**< Register R12 */
    uintptr_t SP;      /**< Stack pointer */
    uintptr_t LR;      /**< Program returning address. */
    uintptr_t PC;      /**< PC pointer of the exceptional function */
};

/*
 * CpuTick结构体类型。
 *
 * 用于记录64位的cycle计数值。
 */
struct SreCpuTick {
    U32 cntHi; /* cycle计数高32位 */
    U32 cntLo; /* cycle计数低32位 */
};

/*
 * 异常信息结构体
 */
struct ExcInfo {
    // OS版本号
    char osVer[OS_SYS_OS_VER_LEN];
    // 产品版本号
    char appVer[OS_SYS_APP_VER_LEN];
    // 异常原因
    U32 excCause;
    // 异常前的线程类型
    U32 threadType;
    // 异常前的线程ID, 该ID组成threadID = LTID
    U32 threadId;
    // 字节序
    U16 byteOrder;
    // CPU类型
    U16 cpuType;
    // CPU ID
    U32 coreId;
    // CPU Tick
    struct SreCpuTick cpuTick;
    // 异常嵌套计数
    U32 nestCnt;
    // 致命错误码，发生致命错误时有效
    U32 fatalErrNo;
    // 异常前栈指针
    uintptr_t sp;
    // 异常前栈底
    uintptr_t stackBottom;
    // 异常发生时的核内寄存器上下文信息
    struct ExcRegInfo regInfo;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* ARMV7_R_EXC_H */
