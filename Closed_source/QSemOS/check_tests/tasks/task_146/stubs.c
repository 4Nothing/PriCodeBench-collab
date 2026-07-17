#include <stdio.h>
#include <stdint.h>
#include "include/prt_raw_spinlock_external.h"
#include "include/prt_task_external.h"

U32 OsSplLockInit(struct PrtSpinLock *spinLock)
{
    if (!spinLock)
        return 1u; // mimic OS_ERRNO_SPL_ALLOC_ADDR_INVALID
    spinLock->rawLock = 0;
    return 0u;
}

void OsSplLock(volatile uintptr_t *addr)
{
    if (addr)
    {
        *addr = 1;
    }
}
void OsSplUnlock(volatile uintptr_t *addr)
{
    if (addr)
    {
        *addr = 0;
    }
}
void OsSplReadLock(volatile uintptr_t *addr)
{
    if (addr)
    {
        (*addr)++;
    }
}
void OsSplReadUnlock(volatile uintptr_t *addr)
{
    if (addr)
    {
        if (*addr)
            (*addr)--;
    }
}
void OsSplWriteLock(volatile uintptr_t *addr)
{
    if (addr)
    {
        *addr = (uintptr_t)-1;
    }
}
void OsSplWriteUnlock(volatile uintptr_t *addr)
{
    if (addr)
    {
        *addr = 0;
    }
}
