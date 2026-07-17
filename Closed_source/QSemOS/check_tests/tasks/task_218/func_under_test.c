OS_SEC_L0_TEXT U32 PRT_TaskDelay(U32 tick)
{
    U32 ret;
    uintptr_t intSave;
    struct TagTskCb *runTask = NULL;

    intSave = OsIntLock();

    if (OS_INT_ACTIVE) {
        OS_REPORT_ERROR(OS_ERRNO_TSK_DELAY_IN_INT);
        OsIntRestore(intSave);
        return OS_ERRNO_TSK_DELAY_IN_INT;
    }

    if (OS_TASK_LOCK_DATA != 0) {
        OsIntRestore(intSave);
        return OS_ERRNO_TSK_DELAY_IN_LOCK;
    }
    runTask = RUNNING_TASK;

#if defined(OS_OPTION_TICK)
    if (tick > 0) {
        OsSpinLockTaskRq(runTask);
        OsTskReadyDel(runTask);
        TSK_STATUS_SET(runTask, OS_TSK_DELAY);
        OsTskTimerAdd(runTask, tick);
        OsSpinUnlockTaskRq(runTask);
        OsTskScheduleFastPs(intSave);
        OsIntRestore(intSave);

        return OS_OK;
    }
#else
    (void)tick;
#endif

#if defined(OS_OPTION_TASK_YIELD)
    ret = OsTaskYield(runTask->priority, OS_TSK_NULL_ID, NULL, intSave);
#else
    ret = OS_OK;
#endif
    OsIntRestore(intSave);

    return ret;
}
