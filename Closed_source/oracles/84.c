OS_SEC_ALW_INLINE INLINE U32 OsSignalWaitSche(struct TagTskCb *runTsk, const signalSet *set, U32 timeOutTick)
{
    if (timeOutTick == 0) {
        return OS_ERRNO_SIGNAL_PARA_INVALID;
    }

    if (OS_TASK_LOCK_DATA != 0) {
        return OS_ERRNO_SIGNAL_WAIT_IN_LOCK;
    }

    runTsk->sigWaitMask = *set;

    OsTskReadyDel(runTsk);

    TSK_STATUS_SET(runTsk, OS_TSK_WAIT_SIGNAL);

    OsSignalTimeOutSet(runTsk, timeOutTick);

    OsSpinUnlockTaskRq(runTsk);
    OsTskSchedule();
    OsSpinLockTaskRq(runTsk);

    /* 任务返回上下文 */
    if ((runTsk->taskStatus & OS_TSK_TIMEOUT) != 0) {
        TSK_STATUS_CLEAR(runTsk, OS_TSK_TIMEOUT);
        return OS_ERRNO_SIGNAL_TIMEOUT;
    }

    return OS_OK;
}