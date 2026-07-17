#pragma once
#include <stdint.h>
#include "prt_perf.h"

struct PrtSpinLock
{
    volatile uintptr_t rawLock;
};

static inline U32 PRT_SplLockInit(volatile struct PrtSpinLock *l)
{
    if (l)
        l->rawLock = 0;
    return OS_OK;
}

static inline uintptr_t PRT_SplIrqLock(volatile struct PrtSpinLock *l)
{
    (void)l;
    return 0; // return dummy irq state
}

static inline void PRT_SplIrqUnlock(volatile struct PrtSpinLock *l, uintptr_t state)
{
    (void)l;
    (void)state;
}
