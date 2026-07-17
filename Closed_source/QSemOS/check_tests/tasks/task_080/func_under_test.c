OS_SEC_ALW_INLINE INLINE U32 OsSigWaitProcPendingSignal(struct TagTskCb *runTsk, const signalSet *set, signalInfo *info)
{
    sigInfoNode *infoNode = NULL;
    LIST_FOR_EACH(infoNode, &runTsk->sigInfoList, sigInfoNode, siglist) {
        if ((sigMask(infoNode->siginfo.si_signo) & *set) == 0) {
            continue;
        }

        (void)memcpy_s(info, sizeof(signalInfo), &(infoNode->siginfo), sizeof(signalInfo));

        /* 清除pending标记 */
        runTsk->sigPending &= ~sigMask(infoNode->siginfo.si_signo);
        ListDelete(&(infoNode->siglist));
        PRT_MemFree((U32)OS_MID_SIGNAL, infoNode);
        break;
    }

    /* 已等到信号，清除等待信号集 */
    runTsk->sigWaitMask = 0;

    return OS_OK;
}