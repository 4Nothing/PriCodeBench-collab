#define OS_SEC_L4_TEXT
U32 PRT_SignalWait(const signalSet *set, signalInfo *info, U32 timeOutTick)
{
    U32 ret = OS_OK;

    if (set == NULL || *set == 0 || info == NULL || timeOutTick == 0) {
        return OS_ERRNO_SIGNAL_PARA_INVALID;
    }

    if (memset_s(info, sizeof(signalInfo), 0, sizeof(signalInfo)) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }

    uintptr_t intSave = OsIntLock();
    if (OS_INT_ACTIVE) {
        OsIntRestore(intSave);
        return OS_ERRNO_SIGNAL_WAIT_NOT_IN_TASK;
    }

    struct TagTskCb *runTsk = RUNNING_TASK;
    OsSpinLockTaskRq(runTsk);

    /* 等待信号集中已有未决信号 */
    if ((runTsk->sigPending & *set) != 0) {
        ret = OsSigWaitProcPendingSignal(runTsk, set, info);
        OsSpinUnlockTaskRq(runTsk);
        OsIntRestore(intSave);
        return ret;
    }

    /* 等待信号集中没有未决信号则需要将当前任务挂起等待 */
    ret = OsSignalWaitSche(runTsk, set, timeOutTick);
    if (ret != OS_OK) {
        OsSpinUnlockTaskRq(runTsk);
        OsIntRestore(intSave);
        return ret;
    }

    ret = OsSigWaitProcPendingSignal(runTsk, set, info);
    OsSpinUnlockTaskRq(runTsk);
    OsIntRestore(intSave);
    return ret;
}
