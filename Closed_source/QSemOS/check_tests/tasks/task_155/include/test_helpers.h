#pragma once

#include "../os_stub.h"

void test_reset(void);
void test_init_task(TagTskCb *tcb, U32 pid, TskPrior priority);
void test_init_sem(TagSemCb *sem, U32 semStat, U32 semType, U32 semCount, U32 semMode);