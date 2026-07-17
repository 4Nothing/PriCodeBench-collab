#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* Basic types */
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t U8;
typedef int32_t S32;

/* Provide test helper functions - actual implementations are in stubs.c */
void test_reset(void);
void test_init_task(void *tcb, U32 pid, U32 prio);
void test_init_sem(void *sem, U32 stat, U32 type, U32 count, U32 mode);