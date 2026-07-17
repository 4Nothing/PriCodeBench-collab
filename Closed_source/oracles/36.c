OS_SEC_L4_TEXT U32 PRT_SemMutexCreate(SemHandle *semHandle)
{
    return OsSemCreate(OS_SEM_FULL, SEM_TYPE_BIN | (SEM_MUTEX_TYPE_RECUR << 4), SEM_MODE_PRIOR, semHandle,
        (U32)(uintptr_t)semHandle);
}
