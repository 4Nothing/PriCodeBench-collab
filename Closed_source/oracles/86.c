OS_SEC_L4_TEXT U32 PRT_SignalAction(int signum, const struct _sigaction *act, struct _sigaction *oldact)
{
    if (!sigValid(signum)) {
        return OS_ERRNO_SIGNAL_NUM_INVALID;
    }

    uintptr_t intSave = OsIntLock();
    struct TagTskCb *runTsk = RUNNING_TASK;
    OsSpinLockTaskRq(runTsk);

    if (oldact != NULL) {
        oldact->saHandler = runTsk->sigVectors[signum];
    }

    if (act == NULL) {
        OsSpinUnlockTaskRq(runTsk);
        OsIntRestore(intSave);
        return OS_OK;
    }

    _sa_handler handler = act->saHandler;
    if (handler == SIG_IGN) {
        runTsk->sigVectors[signum] = NULL;
    } else if (handler == SIG_DFL) {
        runTsk->sigVectors[signum] = OsSigDefaultHandler;
    } else {
        runTsk->sigVectors[signum] = handler;
    }

    OsSpinUnlockTaskRq(runTsk);
    OsIntRestore(intSave);
    return OS_OK;
}
