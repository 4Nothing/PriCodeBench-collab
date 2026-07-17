OS_SEC_L0_TEXT void PRT_SplLock(struct PrtSpinLock *spinLock)
{
    volatile uintptr_t *addr;

    addr = &spinLock->rawLock;

    OS_TSK_SPINLOCK();

    OsSplLock(addr);
}