OS_SEC_L0_TEXT void PRT_TaskUnlock(void)
{
    uintptr_t intSave;
    intSave = OsIntLock();
    OsTaskIrqUnlock(intSave);
}
