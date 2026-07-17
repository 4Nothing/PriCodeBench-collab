OS_SEC_L4_TEXT void OsSignalEntry(TskHandle taskId)
{
    struct TagTskCb *runTsk = RUNNING_TASK;
    int signum = runTsk->holdSignal;
    OsHandleOneSignal(runTsk, signum);

    /* 恢复任务在处理信号之前的上下文，继续执行 */
    runTsk->taskStatus &= ~OS_TSK_HOLD_SIGNAL;
    runTsk->stackPointer = runTsk->oldStackPointer;
    runTsk->holdSignal = 0;
    runTsk->oldStackPointer = NULL;
    OsTskContextLoad((uintptr_t)runTsk);

    return;
}
