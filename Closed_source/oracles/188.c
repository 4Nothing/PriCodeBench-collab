OS_SEC_L4_TEXT void OsTaskNameGet(U32 taskId, char **taskName)
{
    struct TagTskCb *taskCb = GET_TCB_HANDLE(taskId);

    *taskName = taskCb->name;
}
