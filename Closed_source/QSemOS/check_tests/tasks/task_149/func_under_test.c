OS_SEC_L2_TEXT uintptr_t PRT_SplIrqWriteLock(struct PrtSpinLock *spinLock)
{
    uintptr_t intSave;
    volatile uintptr_t *addr = &spinLock->rawLock;
    intSave = PRT_IntLock();
    OS_TSK_LOCK();
    OsSplWriteLock(addr);
    return intSave;
}
