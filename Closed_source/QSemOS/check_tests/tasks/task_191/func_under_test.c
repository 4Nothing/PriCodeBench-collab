OS_SEC_L4_TEXT void OsTskEntry(TskHandle taskId)
{
    struct TagTskCb *taskCb;
    uintptr_t intSave;

    (void)taskId;

    taskCb = OsGetCurrentTcb();

    taskCb->taskEntry(taskCb->args[OS_TSK_PARA_0], taskCb->args[OS_TSK_PARA_1], taskCb->args[OS_TSK_PARA_2],
                      taskCb->args[OS_TSK_PARA_3]);

    intSave = OsIntLock();

    OS_TASK_LOCK_DATA = 0;

    /* PRT_TaskDelete不能关中断操作，否则可能会导致它核发SGI等待本核响应时死等 */
    OsIntRestore(intSave);

    OsTaskExit(taskCb);
}
