OS_SEC_L4_TEXT U32 PRT_SignalMask(int how, const signalSet *set, signalSet *oldset)
{
    uintptr_t intSave = OsIntLock();
    struct TagTskCb *runTsk = RUNNING_TASK;
    OsSpinLockTaskRq(runTsk);
    if (oldset != NULL) {
        *oldset = runTsk->sigMask;
    }

    if (set != NULL) {
        switch(how) {
            case SIG_BLOCK:
                runTsk->sigMask |= *set;
                break;
            case SIG_UNBLOCK:
                runTsk->sigMask &= ~*set;
                OsHandleUnBlockSignal(runTsk);
                break;
            case SIG_SETMASK:
                runTsk->sigMask = *set;
                OsHandleUnBlockSignal(runTsk);
                break;
            default:
                break;
        }
    }

    OsSpinUnlockTaskRq(runTsk);
    OsIntRestore(intSave);
    return OS_OK;
}
