OS_SEC_ALW_INLINE INLINE U32 OsTaskInfoGetChk(TskHandle taskPid, struct TskInfo *taskInfo)
{
    if (taskInfo == NULL) {
        return OS_ERRNO_TSK_PTR_NULL;
    }

    if (CHECK_TSK_PID_OVERFLOW(taskPid)) {
        return OS_ERRNO_TSK_ID_INVALID;
    }
    return OS_OK;
}