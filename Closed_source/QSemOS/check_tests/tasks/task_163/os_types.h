#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int32_t S32;
typedef uint64_t U64;
typedef uint32_t SemHandle;
typedef U32 TskPrior;

#ifndef INLINE
#define INLINE inline
#endif

#define OS_SEC_BSS
#define OS_SEC_ALW_INLINE
#define OS_SEC_L0_TEXT
#define OS_SEC_L4_TEXT

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
