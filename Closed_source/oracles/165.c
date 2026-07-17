OS_SEC_L0_TEXT U32 PRT_SemPend(SemHandle semHandle, U32 timeout)
{
#if defined(OS_OPTION_PERF) && defined(OS_OPTION_PERF_SW_PMU)
    PRT_PERF(SEM_PEND);
#endif
    uintptr_t intSave;
    U32 ret;
    struct TagTskCb *runTsk = NULL;
    struct TagSemCb *semPended = NULL;
    struct TagOsRunQue *runQue = NULL;

    if (semHandle >= (SemHandle)g_maxSem) {
        return OS_ERRNO_SEM_INVALID;
    }

    semPended = GET_SEM(semHandle);

    SEM_CB_IRQ_LOCK(semPended, intSave);

    if (semPended->semStat == OS_SEM_UNUSED) {
        SEM_CB_IRQ_UNLOCK(semPended, intSave);
        return OS_ERRNO_SEM_INVALID;
    }

    if (OS_INT_ACTIVE) {
        SEM_CB_IRQ_UNLOCK(semPended, intSave);
        return OS_ERRNO_SEM_PEND_INTERR;
    }

    runTsk = (struct TagTskCb *)RUNNING_TASK;
    OsSemIfPrioLock(semPended);
    if (OsSemPendNotNeedSche(semPended, runTsk) == TRUE) {
        OsSemIfPrioUnLock(semPended);
        SEM_CB_IRQ_UNLOCK(semPended, intSave);
        return OS_OK;
    }

    ret = OsSemPendParaCheck(timeout);
    if (ret != OS_OK) {
        OsSemIfPrioUnLock(semPended);
        SEM_CB_IRQ_UNLOCK(semPended, intSave);
        return ret;
    }
    /* 把当前任务挂接在信号量链表上 */
    OsSemPendListPut(semPended, timeout);

    OsSemIfPrioUnLock(semPended);
    SEM_CB_UNLOCK(semPended);
    if (timeout != OS_WAIT_FOREVER) {
        /* 触发任务调度 */
        OsTskSchedule();

        runQue = OsSpinLockRunTaskRq();
        /* 判断是否是等待信号量超时 */
        if (TSK_STATUS_TST(runTsk, OS_TSK_TIMEOUT)) {
            TSK_STATUS_CLEAR(runTsk, OS_TSK_TIMEOUT);
            OsSpinUnLockRunTaskRq(runQue);
            OsIntRestore(intSave);
            return OS_ERRNO_SEM_TIMEOUT;
        }
        OsSpinUnLockRunTaskRq(runQue);
    } 
    /* 恢复ps的快速切换 */
    OsTskScheduleFastPs(intSave);
    OsIntRestore(intSave);
    return OS_OK;
}
