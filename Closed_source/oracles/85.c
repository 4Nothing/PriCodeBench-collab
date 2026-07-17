OS_SEC_L4_TEXT U32 PRT_SigSuspend(const signalSet *mask)
{
    U32 ret = OS_OK;
    uintptr_t intSave = OsIntLock();

    struct TagTskCb *runTsk = RUNNING_TASK;
    OsSpinLockTaskRq(runTsk);

    signalSet saveedMask = runTsk->sigMask;
    runTsk->sigMask = *mask;
    runTsk->sigWaitMask = 0;

    signalSet waitMask = ~(*mask);

    /* 等待信号集中已有未屏蔽信号, 则处理未屏蔽信号，返回 */
    if ((runTsk->sigPending & waitMask) != 0) {
        OsHandleUnBlockSignal(runTsk);
        runTsk->sigMask = saveedMask;
        OsSpinUnlockTaskRq(runTsk);
        OsIntRestore(intSave);
    } else {
        if (runTsk->taskPid == IDLE_TASK_ID) {
            OsSpinUnlockTaskRq(runTsk);
            return OS_ERRNO_SIGNAL_TASKID_INVALID;
        }

        OsIntRestore(intSave);
        /* 等待信号集中没有未屏蔽信号则需要将当前任务挂起等待 */
        ret = OsSignalWaitSche(runTsk, &waitMask, OS_SIGNAL_WAIT_FOREVER);
        if (ret != OS_OK) {
            OsSpinUnlockTaskRq(runTsk);
            return ret;
        }

        OsHandleUnBlockSignal(runTsk);
        runTsk->sigMask = saveedMask;
        OsSpinUnlockTaskRq(runTsk);
    }

    return ret;
}
