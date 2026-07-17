#pragma once
#include "os_stub.h"

void test_reset(void);
void test_init_task(struct TagTskCb *tcb, U32 pid, U32 prio);
void test_init_sem(struct TagSemCb *sem, U32 stat, U32 type, U32 count, U32 mode);
void test_register_task(struct TagTskCb *tcb);