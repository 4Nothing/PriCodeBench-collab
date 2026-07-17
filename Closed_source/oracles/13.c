OS_SEC_L4_TEXT U32 PRT_TaskGetPriority(TskHandle taskPid, TskPrior *taskPrio)
{
    uintptr_t intSave;
    struct TagTskCb *taskCb = NULL;

    if (CHECK_TSK_PID_OVERFLOW(taskPid)) {
        return OS_ERRNO_TSK_ID_INVALID;
    }

    if (taskPrio == NULL) {
        return OS_ERRNO_TSK_PTR_NULL;
    }

    taskCb = GET_TCB_HANDLE(taskPid);

    intSave = OsIntLock();

    if (TSK_IS_UNUSED(taskCb)) {
        OsIntRestore(intSave);

        return OS_ERRNO_TSK_NOT_CREATED;
    }

    *taskPrio = taskCb->priority;

    OsIntRestore(intSave);

    return OS_OK;
}
