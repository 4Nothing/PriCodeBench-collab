OS_SEC_L2_TEXT void PRT_SplIrqWriteUnlock(struct PrtSpinLock *spinLock, uintptr_t intSave)
{
    volatile uintptr_t *addr = &spinLock->rawLock;
    OsSplWriteUnlock(addr);
    OsTskUnlock();
    PRT_IntRestore(intSave);
    return;
}
