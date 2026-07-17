OS_SEC_L0_TEXT void PRT_SplUnlock(struct PrtSpinLock *spinLock)
{
    volatile uintptr_t *addr;

    addr = &spinLock->rawLock;

    OsSplUnlock(addr);

    OS_TSK_SPINUNLOCK();
}