OS_SEC_ALW_INLINE INLINE U32 OsTaskPrioritySetCheck(TskHandle taskPid, TskPrior taskPrio)
{
    if (taskPrio > OS_TSK_PRIORITY_LOWEST) {
        return OS_ERRNO_TSK_PRIOR_ERROR;
    }

    if (CHECK_TSK_PID_OVERFLOW(taskPid)) {
        return OS_ERRNO_TSK_ID_INVALID;
    }
    if (taskPid >= g_tskMaxNum) {

        return OS_ERRNO_TSK_OPERATE_IDLE;
    }
    return OS_OK;
}