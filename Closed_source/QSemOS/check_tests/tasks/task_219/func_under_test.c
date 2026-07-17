OS_SEC_L0_TEXT uintptr_t PRT_TaskIrqLock(void)
{
    uintptr_t intSave;

    intSave = OsIntLock();

    OS_TSK_LOCK();
    
    return intSave;
}
