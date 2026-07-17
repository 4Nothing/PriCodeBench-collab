OS_SEC_ALW_INLINE INLINE U32 OsAddSignalPendingFlag(struct TagTskCb *taskCb, signalInfo *info)
{
    sigInfoNode *infoNode = NULL;
    /* 若未决信号已存在，则更新未决信号对应的数据 */
    if ((taskCb->sigPending & sigMask(info->si_signo)) != 0) {
        LIST_FOR_EACH(infoNode, &taskCb->sigInfoList, sigInfoNode, siglist) {
            if (infoNode->siginfo.si_signo != info->si_signo) {
                continue;
            }

            (void)memcpy_s(&infoNode->siginfo, sizeof(signalInfo), info, sizeof(signalInfo));
            break;
        }
        return OS_OK;
    }

    infoNode = (sigInfoNode *)PRT_MemAlloc((U32)OS_MID_SIGNAL, OS_MEM_DEFAULT_FSC_PT, sizeof(sigInfoNode));
    if (infoNode == NULL) {
        return OS_ERRNO_SIGNAL_NO_MEMORY;
    }
    
    (void)memcpy_s(&infoNode->siginfo, sizeof(signalInfo), info, sizeof(signalInfo));
    ListTailAdd(&infoNode->siglist, &taskCb->sigInfoList);
    taskCb->sigPending |= sigMask(info->si_signo);

    return OS_OK;
}