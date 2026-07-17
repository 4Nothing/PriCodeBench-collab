/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Aarch32 Atomic HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef OS_ATOMIC_ARMV7_R_H
#define OS_ATOMIC_ARMV7_R_H

#include "prt_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef S32     INT32;
typedef S64     INT64;
typedef bool    BOOL;
typedef U32     UINT32;
typedef void    VOID;
typedef volatile INT32 Atomic;
typedef volatile INT64 Atomic64;

#define STATIC

STATIC INLINE INT32 ArchAtomicRead(const Atomic *v)
{
    return *(volatile INT32 *)v;
}

STATIC INLINE VOID ArchAtomicSet(Atomic *v, INT32 setVal)
{
    *(volatile INT32 *)v = setVal;
}

STATIC INLINE INT32 ArchAtomicAdd(Atomic *v, INT32 addVal)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %1, [%2]\n"
                             "add   %1, %1, %3\n"
                             "strex   %0, %1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(addVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE INT32 ArchAtomicSub(Atomic *v, INT32 subVal)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %1, [%2]\n"
                             "sub   %1, %1, %3\n"
                             "strex   %0, %1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(subVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE VOID ArchAtomicInc(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "add   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

STATIC INLINE INT32 ArchAtomicIncRet(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "add   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE VOID ArchAtomicDec(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "sub   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

STATIC INLINE INT32 ArchAtomicDecRet(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "sub   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE INT64 ArchAtomic64Read(const Atomic64 *v)
{
    INT64 val;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%1]"
                             : "=&r"(val)
                             : "r"(v)
                             : "cc");
    } while (0);

    return val;
}

STATIC INLINE VOID ArchAtomic64Set(Atomic64 *v, INT64 setVal)
{
    INT64 tmp;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %1, %H1, [%2]\n"
                             "strexd   %0, %3, %H3, [%2]"
                             : "=&r"(status), "=&r"(tmp)
                             : "r"(v), "r"(setVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

STATIC INLINE INT64 ArchAtomic64Add(Atomic64 *v, INT64 addVal)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %1, %H1, [%2]\n"
                             "adds   %Q1, %Q1, %Q3\n"
                             "adc   %R1, %R1, %R3\n"
                             "strexd   %0, %1, %H1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(addVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE INT64 ArchAtomic64Sub(Atomic64 *v, INT64 subVal)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %1, %H1, [%2]\n"
                             "subs   %Q1, %Q1, %Q3\n"
                             "sbc   %R1, %R1, %R3\n"
                             "strexd   %0, %1, %H1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(subVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE VOID ArchAtomic64Inc(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "adds   %Q0, %Q0, #1\n"
                             "adc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

STATIC INLINE INT64 ArchAtomic64IncRet(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "adds   %Q0, %Q0, #1\n"
                             "adc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE VOID ArchAtomic64Dec(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "subs   %Q0, %Q0, #1\n"
                             "sbc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

STATIC INLINE INT64 ArchAtomic64DecRet(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "subs   %Q0, %Q0, #1\n"
                             "sbc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

STATIC INLINE INT32 ArchAtomicXchg32bits(Atomic *v, INT32 val)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "strex   %1, %4, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal;
}

STATIC INLINE INT64 ArchAtomicXchg64bits(Atomic64 *v, INT64 val)
{
    INT64 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "strexd   %1, %4, %H4, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal;
}

STATIC INLINE BOOL ArchAtomicCmpXchg32bits(Atomic *v, INT32 val, INT32 oldVal)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex %0, [%3]\n"
                             "mov %1, #0\n"
                             "teq %0, %4\n"
                             "strexeq %1, %5, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal != oldVal;
}

STATIC INLINE BOOL ArchAtomicCmpXchg64bits(Atomic64 *v, INT64 val, INT64 oldVal)
{
    INT64 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "mov   %1, #0\n"
                             "teq   %0, %4\n"
                             "teqeq   %H0, %H4\n"
                             "strexdeq   %1, %5, %H5, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal != oldVal;
}

OS_SEC_ALW_INLINE INLINE U64 OsLdrExd(volatile void *ptr)
{
    (void)ptr;
    return 0;
}

OS_SEC_ALW_INLINE INLINE S32 OsStrExd(U64 val, volatile void *ptr)
{
    (void)ptr;
    return (S32)val;
}


/*
 * @brief 有符号32位变量的原子加，并返回累加后的值
 *
 * @par 描述
 * 有符号32位变量的原子加，并返回累加后的值
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S32*, 要累加变量的地址。
 * @param[in]       incr 类型#S32, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAdd32Rtn| PRT_AtomicFetchAndAdd32
 */
OS_SEC_ALW_INLINE INLINE S32 PRT_AtomicAdd32Rtn(S32 *ptr, S32 incr)
{
    return ArchAtomicAdd((Atomic *)ptr, incr);
}

/*
 * @brief 无符号32位变量的原子加，并返回累加后的值
 *
 * @par 描述
 * 有符号32位变量的原子加，并返回累加后的值
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S32*, 要累加变量的地址。
 * @param[in]       incr 类型#S32, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddU32| PRT_AtomicFetchAndAddU32
 */
OS_SEC_ALW_INLINE INLINE U32 PRT_AtomicAddU32Rtn(U32 *ptr, U32 incr)
{
    return (U32)ArchAtomicAdd((Atomic *)ptr, (S32)incr);
}


/*
 * @brief 有符号64位变量的原子加，并返回累加后的值
 *
 * @par 描述
 * 有符号32位变量的原子加，并返回累加后的值
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S64*, 要累加变量的地址。
 * @param[in]       incr 类型#S64, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAdd64Rtn| PRT_AtomicFetchAndAdd64
 */
OS_SEC_ALW_INLINE INLINE S64 PRT_AtomicAdd64Rtn(S64 *atomic, S64 incr)
{
    return ArchAtomic64Add((Atomic64 *)atomic, incr);
}

/*
 * @brief 无符号64位变量的原子加，并返回累加后的值
 *
 * @par 描述
 * 有符号64位变量的原子加，并返回累加后的值
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S64*, 要累加变量的地址。
 * @param[in]       incr 类型#S64, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddU64 | PRT_AtomicFetchAndAddU64
 */
OS_SEC_ALW_INLINE INLINE U64 PRT_AtomicAddU64Rtn(U64 *atomic, U64 incr)
{
    return (U64)ArchAtomic64Add((Atomic64 *)atomic, (S64)incr);
}

/*
 * @brief 有符号32位变量的原子加
 *
 * @par 描述
 * 有符号32位变量的原子加
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S32*, 要累加变量的地址。
 * @param[in]       incr 类型#S32, 要累加的数值。
 *
 * @retval 无
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddS32Rtn| PRT_AtomicFetchAndAddS32
 */
OS_SEC_ALW_INLINE INLINE void PRT_AtomicAdd32(S32 *ptr, S32 incr)
{
    (void)ArchAtomicAdd((Atomic *)ptr, incr);
}

/*
 * @brief 无符号32位变量的原子加
 *
 * @par 描述
 * 有符号32位变量的原子加
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S32*, 要累加变量的地址。
 * @param[in]       incr 类型#S32, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddU32Rtn| PRT_AtomicFetchAndAddU32
 */
OS_SEC_ALW_INLINE INLINE void PRT_AtomicAddU32(U32 *ptr, U32 incr)
{
    (void)ArchAtomicAdd((Atomic *)ptr, (S32)incr);
}

/*
 * @brief 有符号64位变量的原子加
 *
 * @par 描述
 * 有符号32位变量的原子加
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S64*, 要累加变量的地址。
 * @param[in]       incr 类型#S64, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddS64| PRT_AtomicFetchAndAddS64
 */
OS_SEC_ALW_INLINE INLINE void PRT_AtomicAddS64(S64 *atomic, S64 incr)
{
    (void)ArchAtomic64Add((Atomic64 *)atomic, incr);
}

/*
 * @brief 无符号64位变量的原子加
 *
 * @par 描述
 * 有符号64位变量的原子加
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S64*, 要累加变量的地址。
 * @param[in]       incr 类型#S64, 要累加的数值。
 *
 * @retval 变量累加后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddU64Rtn | PRT_AtomicFetchAndAddU64
 */
OS_SEC_ALW_INLINE INLINE void PRT_AtomicAddU64(U64 *atomic, U64 incr)
{
    (void)ArchAtomic64Add((Atomic64 *)atomic, (S64)incr);
}


/*
 * @brief 32位原子交换，并返回交换前的值
 *
 * @par 描述
 * 32位原子交换，并返回交换前的值
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U32*, 要交换变量的地址。
 * @param[in]       newValue 类型#U32，要交换的值。
 *
 * @retval 变量交换前的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicSwap64
 */
OS_SEC_ALW_INLINE INLINE U32 PRT_AtomicSwap32(U32 *ptr, U32 newValue)
{
    return (U32)ArchAtomicXchg32bits((Atomic *)ptr, (S32)newValue);
}

/*
 * @brief 64位原子交换，并返回交换前的值
 *
 * @par 描述
 * 64位原子交换，并返回交换前的值
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U64*, 要交换变量的地址。
 * @param[in]       newValue 类型#U64，要交换的值。
 *
 * @retval 变量交换前的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicSwap64
 */
OS_SEC_ALW_INLINE INLINE U64 PRT_AtomicSwap64(U64 *ptr, U64 newValue)
{
    return (U64)ArchAtomicXchg64bits((Atomic64 *)ptr, (S64)newValue);
}

/*
 * @brief 完成32位无符号的变量与指定内存的值比较，并在相等的情况下赋值
 *
 * @par 描述
 * 完成32位无符号的变量与指定内存的值比较，并在相等的情况下赋值，相等时赋值并返回1，不相等时返回零。
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U32*, 要比较/赋值变量的地址。
 * @param[in]       oldVal 类型#U32，要比较的值。
 * @param[in]       newVal 类型#U32，相等时要写入的新值。
 *
 * @retval 1，相等并赋值
 * @retval 0，不相等。
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicCompareAndStore64
 */
OS_SEC_ALW_INLINE INLINE U32 PRT_AtomicCompareAndStore32(U32 *ptr, U32 oldVal, U32 newVal)
{
    if(ArchAtomicCmpXchg32bits((Atomic *)ptr, (INT32)oldVal, (INT32)newVal))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


/*
 * @brief 有符号32位变量的原子加，并返回累加前的值
 *
 * @par 描述
 * 有符号32位变量的原子加，并返回累加前的值
 * 
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#S32*, 要累加变量的地址。
 * @param[in]       incr 类型#S32, 要累加的数值。
 *
 * @retval 变量累加前的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddS32| PRT_AtomicAddS32Rtn
 */
OS_SEC_ALW_INLINE INLINE S32 PRT_AtomicFetchAndAddS32(S32 *ptr, S32 incr){
    return ArchAtomicAdd((Atomic *)ptr, incr);
}

/*
 * @brief 无符号的32位原子或操作
 *
 * @par 描述
 * 无符号的32位原子或操作
 * 
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  addr 类型#U32*, 要执行逻辑或变量的地址。
 * @param[in]       incr 类型#U32, 要逻辑或的值。
 *
 * @retval 执行或操作以后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see
 */
OS_SEC_ALW_INLINE INLINE U32 PRT_AtomicOr(U32 *addr, U32 val)
{
    U32 ret = *addr;

    *addr = ret | val;

    return ret;
}

/*
 * @brief 无符号的32位原子与操作
 *
 * @par 描述
 * 无符号的32位原子与操作
 * 
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  addr 类型#U32*, 要执行逻辑与变量的地址。
 * @param[in]       incr 类型#U32, 要逻辑与的值。
 *
 * @retval 执行或操作以后的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see
 */
OS_SEC_ALW_INLINE INLINE U32 PRT_AtomicAnd(U32 *addr, U32 val)
{
    U32 ret = *addr;

    *addr = ret & val;

    return ret;
}

/*
 * @brief 完成64位无符号的变量与指定内存的值比较，并在相等的情况下赋值
 *
 * @par 描述
 * 完成64位无符号的变量与指定内存的值比较，并在相等的情况下赋值，相等时赋值并返回1，不相等时返回零。
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U64*, 要比较/赋值变量的地址。
 * @param[in]       oldVal 类型#U64，要比较的值。
 * @param[in]       newVal 类型#U64，相等时要写入的新值。
 *
 * @retval 1，相等并赋值
 * @retval 0，不相等。
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicCompareAndStore64
 */
OS_SEC_ALW_INLINE INLINE U64 PRT_AtomicCompareAndStore64(U64 *ptr, U64 oldVal, U64 newVal)
{
    if(ArchAtomicCmpXchg64bits((Atomic64 *)ptr, (INT64)newVal, (INT64)oldVal)){
        return 0;
    }
    return 1;
}

/*
 * @brief 无符号32位变量的原子加，并返回累加前的值
 *
 * @par 描述
 * 无符号32位变量的原子加，并返回累加前的值
 * 
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U32*, 要累加变量的地址。
 * @param[in]       incr 类型#U32, 要累加的数值。
 *
 * @retval 变量累加前的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddU32| PRT_AtomicAddU32Rtn
 */
OS_SEC_ALW_INLINE INLINE U32 PRT_AtomicFetchAndAddU32(U32 *ptr, U32 incr)
{
    // OsArmPrefecthw(ptr);
    return (U32)ArchAtomicAdd((Atomic *)ptr, (S32)incr);
}

/*
 * @brief 无符号64位变量的原子加，并返回累加前的值
 *
 * @par 描述
 * 无符号64位变量的原子加，并返回累加前的值
 * 
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U64*, 要累加变量的地址。
 * @param[in]       incr 类型#U64, 要累加的数值。
 *
 * @retval 变量累加前的值
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicAddU64| PRT_AtomicAddU64Rtn
 */
OS_SEC_ALW_INLINE INLINE U64 PRT_AtomicFetchAndAddU64(U64 *ptr, U64 incr)
{
    // OsArmPrefecthw(ptr);
    return (U64)ArchAtomic64Add((Atomic64 *)ptr, (S64)incr);
}

/*
 * @brief 无符号64位原子读
 *
 * @par 描述
 * 无符号64位原子读
 *
 * @attention
 * 在部分的CPU或DSP上用特殊指令实现，性能较高。
 *
 * @param[in, out]  ptr 类型#U64*, 要读取的地址。
 *
 * @retval 读取的数据
 * @par 依赖
 * <ul><li>prt_atomic.h：该接口声明所在的头文件。</li></ul>
 * @see PRT_AtomicRead32 | PRT_AtomicReadU32 | PRT_AtomicRead64
 */
OS_SEC_ALW_INLINE INLINE U64 PRT_AtomicReadU64(U64 *ptr)
{
    return ArchAtomic64Read((const Atomic64 *)ptr);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* OS_ATOMIC_ARMV7_R_H */

