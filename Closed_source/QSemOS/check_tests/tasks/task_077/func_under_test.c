OS_SEC_L4_TEXT void OsHandleUnBlockSignal(struct TagTskCb *runTsk)
{
    signalSet unBlockSignal = (runTsk->sigPending & ~(runTsk->sigMask));
    if (unBlockSignal == 0) {
        return;
    }

    sigInfoNode *infoNode = NULL;
    sigInfoNode *tmpNode = NULL;
    LIST_FOR_EACH(infoNode, &runTsk->sigInfoList, sigInfoNode, siglist) {
        int signum = infoNode->siginfo.si_signo;
        if ((unBlockSignal & sigMask(signum)) == 0) {
            continue;
        }

        _sa_handler handler = runTsk->sigVectors[signum];
        if (handler != NULL) {
            handler(signum);
        }

        /* 清除pending标记 */
        runTsk->sigPending &= ~sigMask(signum);
        tmpNode = LIST_COMPONENT(infoNode->siglist.prev, sigInfoNode, siglist);
        ListDelete(&(infoNode->siglist));
        PRT_MemFree((U32)OS_MID_SIGNAL, infoNode);
        infoNode = tmpNode;

        unBlockSignal = (runTsk->sigPending & ~(runTsk->sigMask));
        if (unBlockSignal == 0) {
            break;
        }
    }

    return;
}
