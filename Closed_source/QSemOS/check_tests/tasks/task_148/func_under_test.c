OS_SEC_L0_TEXT void PRT_SplIrqUnlock(struct PrtSpinLock *spinLock, uintptr_t intSave)
{
    volatile uintptr_t *addr;

    addr = &spinLock->rawLock;

    OsSplUnlock(addr);
    OS_TSK_SPINUNLOCK();
    OsIntRestore(intSave);
}