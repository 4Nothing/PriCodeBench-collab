OS_SEC_L0_TEXT void PRT_TaskLock(void)
{
    uintptr_t intSave;

    intSave = OsIntLock();

    OS_TSK_LOCK();

    OsIntRestore(intSave);
}
