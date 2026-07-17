static OS_SEC_L2_TEXT U32 OsTaskYield(TskPrior taskPrio, TskHandle nextTaskId, TskHandle *yieldTo, uintptr_t intSave)
{
    U32 ret;
    U32 tskCount = 0;
    struct TagTskCb *currTask = NULL;
    struct TagTskCb *runTask = NULL;
    struct TagListObject *tskPriorRdyList = NULL;
    struct TagListObject *currNode = NULL;
    runTask = RUNNING_TASK;

    OsSpinLockTaskRq(runTask);
    tskPriorRdyList = OS_TASK_GET_PRI_LIST(taskPrio);
    /* In case there are more then one ready tasks at */
    /* this priority, remove first task and add it */
    /* to the end of the queue */
    currTask = GET_TCB_PEND(OS_LIST_FIRST(tskPriorRdyList));

    for (currNode = tskPriorRdyList->next; currNode != tskPriorRdyList; currNode = currNode->next) {
        tskCount++;
    }

    if (tskCount > 1) {
        ret = OsTaskYieldProc(nextTaskId, taskPrio, yieldTo, currTask, tskPriorRdyList);
        if (ret != OS_OK) {
            OsSpinUnlockTaskRq(runTask);
            return ret;
        }

        if (TSK_STATUS_TST(currTask, OS_TSK_RUNNING)) {
            OsRescheduleCurr(THIS_RUNQ());
            OsSpinUnlockTaskRq(runTask);
            OsTskScheduleFast();
            return OS_OK;
        }
    } else { /* There is only one task or none */
        OsSpinUnlockTaskRq(runTask);
        return OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK;
    }

    OsSpinUnlockTaskRq(runTask);
    return OS_OK;
}
