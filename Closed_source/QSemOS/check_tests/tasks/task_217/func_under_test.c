OS_SEC_ALW_INLINE INLINE U32 OsTaskYieldProc(TskHandle nextTaskId, U32 taskPrio, TskHandle *yieldTo,
                                             struct TagTskCb *currTask,
                                             struct TagListObject *tskPriorRdyList)
{
    struct TagTskCb *nextTask = NULL;
    /* there is no task that user particularly wishes */
    /* therefore second task will become ready */
    if (nextTaskId == OS_TSK_NULL_ID) {
        if (yieldTo != NULL) {
            *yieldTo = (GET_TCB_PEND(OS_LIST_FIRST(&currTask->pendList)))->taskPid;
        }
        ListDelete(&currTask->pendList);
        ListTailAdd(&currTask->pendList, tskPriorRdyList);
    } else {
        if (yieldTo != NULL) {
            *yieldTo = nextTaskId;
        }

        nextTask = GET_TCB_HANDLE(nextTaskId);
        if ((nextTask->priority == taskPrio) && TSK_STATUS_TST(nextTask, OS_TSK_READY)) {
            ListDelete(&nextTask->pendList);
            ListAdd(&nextTask->pendList, tskPriorRdyList);
        } else { /* task is illegal */
            return OS_ERRNO_TSK_YIELD_INVALID_TASK;
        }
    }

    return OS_OK;
}