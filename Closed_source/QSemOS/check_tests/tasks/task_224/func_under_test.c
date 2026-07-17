OS_SEC_L2_TEXT U32 PRT_TaskYield(TskPrior taskPrio, TskHandle nextTask, TskHandle *yieldTo)
{
    uintptr_t intSave;
    U32 ret;

    if (taskPrio > OS_TSK_PRIORITY_LOWEST) {
        return OS_ERRNO_TSK_PRIOR_ERROR;
    }

    if ((nextTask != OS_TSK_NULL_ID) && (CHECK_TSK_PID_OVERFLOW(nextTask))) {
        return OS_ERRNO_BUILD_ID_INVALID;
    }

    intSave = OsIntLock();
    ret = OsTaskYield(taskPrio, nextTask, yieldTo, intSave);
    OsIntRestore(intSave);
    return ret;
}
