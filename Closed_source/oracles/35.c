OS_SEC_L4_TEXT U32 PRT_SemCreate(U32 count, SemHandle *semHandle)
{
    U32 ret;

    if (count > OS_SEM_COUNT_MAX) {
        return OS_ERRNO_SEM_OVERFLOW;
    }

    ret = OsSemCreate(count, SEM_TYPE_COUNT, SEM_MODE_FIFO, semHandle, (U32)(uintptr_t)semHandle);
    return ret;
}
