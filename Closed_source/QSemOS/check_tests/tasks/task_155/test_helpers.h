#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t U8;
typedef int32_t S32;

void test_reset(void);
void test_init_task(struct TagTskCb *tcb, U32 pid, U32 prio);
void test_init_sem(struct TagSemCb *sem, U32 stat, U32 type, U32 count, U32 mode);
