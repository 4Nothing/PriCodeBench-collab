extern inline void OsHandleOneSignal(struct TagTskCb *runTsk, int signum);

OS_SEC_ALW_INLINE INLINE void OsHandleOneSignal(struct TagTskCb *runTsk, int signum)
{
    sigInfoNode *infoNode = NULL;
    LIST_FOR_EACH(infoNode, &runTsk->sigInfoList, sigInfoNode, siglist) {
        if (infoNode->siginfo.si_signo != signum) {
            continue;
        }

        _sa_handler handler = runTsk->sigVectors[signum];
        if (handler != NULL) {
            handler(signum);
        }

        /* 清除pending标记 */
        OsSpinLockTaskRq(runTsk);
        runTsk->sigPending &= ~sigMask(signum);
        OsSpinUnlockTaskRq(runTsk);
        ListDelete(&(infoNode->siglist));
        PRT_MemFree((U32)OS_MID_SIGNAL, infoNode);
        break;
    }
    return;
}