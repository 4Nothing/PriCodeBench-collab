OS_SEC_L2_TEXT void PRT_SplIrqReadUnlock(struct PrtSpinLock *spinLock, uintptr_t intSave)
{
    volatile uintptr_t *addr = &spinLock->rawLock;
    OsSplReadUnlock(addr);
    OsTskUnlock();
    PRT_IntRestore(intSave);
    return;
}
