#include "auto_stub.h"
#include "stubs/prt_signal.h"

/* 辅助函数来自 task_075: OsAddSignalPendingFlag */
U32 OsAddSignalPendingFlag(struct TagTskCb *taskCb, signalInfo *info)
{
    U32 intSave;
    sigInfoNode *infoNode = NULL;

    if ((taskCb->sigPending & sigMask(info->si_signo)) != 0) {
        return OS_OK;
    }

    intSave = OsIntLock();
    if ((taskCb->sigPending & sigMask(info->si_signo)) != 0) {
        OsIntRestore(intSave);
        return OS_OK;
    }

    infoNode = (sigInfoNode *)PRT_MemAlloc((U32)OS_MID_SIGNAL, OS_MEM_DEFAULT_FSC_PT, sizeof(sigInfoNode));
    if (infoNode == NULL) {
        OsIntRestore(intSave);
        return OS_ERRNO_SIGNAL_NO_MEMORY;
    }

    infoNode->siginfo.si_signo = info->si_signo;
    infoNode->siginfo.si_code = info->si_code;
    ListTailAdd(&infoNode->siglist, &taskCb->sigInfoList);
    taskCb->sigPending |= sigMask(info->si_signo);

    OsIntRestore(intSave);
    return OS_OK;
}

/* 辅助函数来自 task_080: OsSigWaitProcPendingSignal */
U32 OsSigWaitProcPendingSignal(struct TagTskCb *runTsk, const signalSet *set, signalInfo *info)
{
    sigInfoNode *infoNode = NULL;
    struct TagTskCb *taskCb = runTsk;

    /* 遍历该任务的所有已到达信号 */
    LIST_FOR_EACH(infoNode, &taskCb->sigInfoList, sigInfoNode, siglist) {
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

/* 辅助函数来自 task_082: OsSignalTimeOutSet */
U32 OsSignalTimeOutSet(struct TagTskCb *taskCb, U32 timeout)
{
    (void)taskCb;
    (void)timeout;
    /* 非SMP版本不需要设置超时，直接返回OK */
    return OS_OK;
}

/* 辅助函数来自 task_084: OsSignalWaitSche */
U32 OsSignalWaitSche(struct TagTskCb *runTsk, const signalSet *set, U32 timeOutTick)
{
    (void)runTsk;
    (void)set;
    (void)timeOutTick;
    /* 非SMP版本不需要调度，直接返回OK */
    return OS_OK;
}
