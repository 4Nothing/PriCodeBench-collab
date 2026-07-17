OS_SEC_ALW_INLINE INLINE void OsTaskInfoCommonGet(struct TskInfo *taskInfo, struct TagTskCb *taskCb, TskHandle taskPid)
{
    char *name = NULL;

    taskInfo->taskStatus = (TskStatus)taskCb->taskStatus;
    taskInfo->taskPrio = taskCb->priority;
    taskInfo->stackSize = taskCb->stackSize;
    taskInfo->topOfStack = taskCb->topOfStack;

    if (g_taskNameGet != NULL) {
        g_taskNameGet(taskPid, &name);

        if (strncpy_s(taskInfo->name, sizeof(taskInfo->name), name, (sizeof(taskInfo->name) - 1)) != EOK) {
            OS_GOTO_SYS_ERROR1();
        }
        taskInfo->name[OS_TSK_NAME_LEN - 1] = '\0';
    }

    taskInfo->core = taskCb->coreID;

    taskInfo->entry = taskCb->taskEntry;
    taskInfo->tcbAddr = taskCb;
}