/*
 * QSemOS Type Definitions for Linux Testing
 * 
 * 提供 QSemOS 所需的基本类型定义，适配 Linux gcc 环境。
 * 基于 src/include/uapi/prt_typedef.h 和相关头文件。
 */

#ifndef __QSEMOS_TYPES_H__
#define __QSEMOS_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Note: uintptr_t is already defined in stdint.h */

/* Boolean types */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Error codes base */
#define OS_OK 0
#define OS_ERROR (~0U)

/* Handle types */
typedef uint32_t TskHandle;
typedef uint32_t SemHandle;
typedef uint32_t TimerHandle;
typedef uint32_t HwiHandle;
typedef uint32_t QueueHandle;
typedef uint32_t ModHandle;
typedef uint32_t TimerGroupId;
typedef uint32_t TmrHandle;

/* Priority type */
typedef uint32_t TskPrior;

/* Status type */
typedef uint32_t UssRet;
typedef uint32_t OsRet;

/* Basic types - alias to standard types */
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

/* Common constants */
#define U32_MAX 0xFFFFFFFFU
#define U16_MAX 0xFFFFU
#define U8_MAX 0xFFU
#define U12_INVALID 0xFFFU
#define U32_INVALID 0xFFFFFFFFU

/* Memory pool IDs */
#define OS_MID_PERF 0
#define OS_MEM_DEFAULT_PT0 0

/* Performance related */
#define PERF_BUFFER_WATERMARK_ONE_N 2

/* Inline macro */
#ifdef __GNUC__
#define OS_SEC_ALW_INLINE inline __attribute__((always_inline))
#else
#define OS_SEC_ALW_INLINE inline
#endif

/* Section macros - strip for Linux */
#define OS_SEC_BSS
#define OS_SEC_L4_TEXT
#define OS_SEC_L2_TEXT
#define OS_SEC_TEXT

/* Static inline for Linux */
#ifdef __GNUC__
#define STATIC static inline __attribute__((always_inline))
#else
#define STATIC static inline
#endif

/* Attribute macros */
#define OS_ATTR_EXTERN_INC_SPEC

#endif /* __QSEMOS_TYPES_H__ */
