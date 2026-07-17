OS_SEC_L0_TEXT U32 OsSemPendParaCheck(U32 timeout)
{
    if (timeout == 0) {
        return OS_ERRNO_SEM_UNAVAILABLE;
    }

    if (OS_TASK_LOCK_DATA != 0) {
        return OS_ERRNO_SEM_PEND_IN_LOCK;
    }
    return OS_OK;
}
