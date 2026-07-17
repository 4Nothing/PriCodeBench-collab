OS_SEC_L4_TEXT U32 PRT_SignalDeliver(TskHandle taskId, signalInfo *info)
{
    if (CHECK_TSK_PID_OVERFLOW(taskId) || info == NULL) {
        return OS_ERRNO_SIGNAL_PARA_INVALID;
    }

    int signum = info->si_signo;
    if (!sigValid(signum)) {
        return OS_ERRNO_SIGNAL_NUM_INVALID;
    }

    uintptr_t intSave = OsIntLock();
    struct TagTskCb *taskCb = (struct TagTskCb *)GET_TCB_HANDLE(taskId);
    OsSpinLockTaskRq(taskCb);
    if (TSK_IS_UNUSED(taskCb)) {
        OsSpinUnlockTaskRq(taskCb);
        OsIntRestore(intSave);
        return OS_ERRNO_SIGNAL_TSK_NOT_CREATED;
    }

    /* 将信号加入pending中，若已存在则更新，不存在则创建 */
    U32 ret = OsAddSignalPendingFlag(taskCb, info);
    if (ret != OS_OK) {
        OsSpinUnlockTaskRq(taskCb);
        OsIntRestore(intSave);
        return ret;
    }

    /* 任务是否在等待该信号，若是则将该任务加入ready队列，触发一次调度 */
    if (((taskCb->taskStatus & OS_TSK_WAIT_SIGNAL) != 0) && ((taskCb->sigWaitMask & sigMask(signum)) != 0)) {
        TSK_STATUS_CLEAR(taskCb, OS_TSK_WAIT_SIGNAL);
        OsSignalWaitReSche(taskCb, taskCb->taskStatus);
        OsIntRestore(intSave);
        return OS_OK;
    }

    /* 信号是否为阻塞信号，若是则不返回不处理 */
    if ((taskCb->sigMask & sigMask(signum)) != 0) {
        OsSpinUnlockTaskRq(taskCb);
        OsIntRestore(intSave);
        return OS_OK;
    }

    /* 判断信号是否是当前的任务处理，若是则处理 */
    if (taskId == RUNNING_TASK->taskPid) {
        OsSpinUnlockTaskRq(taskCb);
        OsHandleOneSignal(taskCb, signum);
        OsIntRestore(intSave);
        return OS_OK;
    }

    /* 若任务在就绪队列或者被延时且没有在等待调度去处理信号，则将该任务的上下文保存并使下次调度直接进入信号处理函数*/
    if ((taskCb->taskStatus & (OS_TSK_READY | OS_TSK_DELAY | OS_TSK_SIG_PAUSE | OS_TSK_DELAY_INTERRUPTIBLE)) &&
        !(taskCb->taskStatus & OS_TSK_HOLD_SIGNAL)) {
        taskCb->taskStatus |= OS_TSK_HOLD_SIGNAL;

        taskCb->oldStackPointer = taskCb->stackPointer;
        taskCb->holdSignal = signum;
        U32 curStackSize = (U32)taskCb->stackPointer - (U32)taskCb->topOfStack;
        taskCb->stackPointer = OsTskContextInit(taskId, curStackSize, (uintptr_t *)taskCb->topOfStack,
            (uintptr_t)OsSignalEntry);
        /* 如果在暂停状态，收到信号后任务加回就绪队列*/
        if (taskCb->taskStatus & OS_TSK_SIG_PAUSE) {
            taskCb->taskStatus &= ~OS_TSK_SIG_PAUSE;
            OsTskReadyAdd(taskCb);
        }
#if defined(OS_OPTION_LINUX)
        /* 任务如果在TASK_INTERRUPTIBLE状态收到信号会唤醒*/
        else if (KTHREAD_TSK_STATE_TST(taskCb, TASK_INTERRUPTIBLE)) {
            wake_up_process(taskCb->kthreadTsk);
        }
#endif
        OsSpinUnlockTaskRq(taskCb);

        OsTskSchedule();
    } else {
        OsSpinUnlockTaskRq(taskCb);
    }

    OsIntRestore(intSave);
    return OS_OK;
}
