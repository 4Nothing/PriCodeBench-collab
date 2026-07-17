OS_SEC_L4_TEXT U32 PRT_TaskSetPriority(TskHandle taskPid, TskPrior taskPrio)
{
    U32 ret;
    bool isReady = FALSE;
    uintptr_t intSave;
    struct TagTskCb *taskCb = NULL;
    U32 semPrioLockFlag;

    ret = OsTaskPrioritySetCheck(taskPid, taskPrio);
    if (ret != OS_OK) {
        return ret;
    }

    taskCb = GET_TCB_HANDLE(taskPid);
    intSave = OsIntLock();
    // 如果持有信号量互斥锁，避免遍历semBList时和semBList产生变化，需要锁semIfPrio
    semPrioLockFlag = OsSemPrioBListLock(taskCb);
    OsSpinLockTaskRq(taskCb);
    if (TSK_IS_UNUSED(taskCb)) {
        OsIntRestore(intSave);
        OsSpinUnlockTaskRq(taskCb);
        OsSemPrioBListUnLock(semPrioLockFlag);
        return OS_ERRNO_TSK_NOT_CREATED;
    }

#if defined(OS_OPTION_SEM_PRIO_INHERIT)
    if (g_checkPrioritySet != NULL) {
        ret = g_checkPrioritySet(taskCb, taskPrio);
        if (ret != OS_OK) {
            OsSpinUnlockTaskRq(taskCb);
            OsSemPrioBListUnLock(semPrioLockFlag);
            OsIntRestore(intSave);
            return ret;
        }
    }
#endif

    isReady = (OS_TSK_READY & taskCb->taskStatus);

    /* delete the task & insert with right priority into ready queue */
    if (isReady) {
        OsTskReadyDel(taskCb);
        taskCb->priority = taskPrio;
        OsTskReadyAdd(taskCb);
    } else {
        taskCb->priority = taskPrio;
    }
    taskCb->origPriority = taskPrio;
    OsSpinUnlockTaskRq(taskCb);
    OsSemPrioBListUnLock(semPrioLockFlag);

    /* reschedule if ready changed */
    if (isReady) {
        OsTskSchedule();
    }

    OsIntRestore(intSave);
    return OS_OK;
}
