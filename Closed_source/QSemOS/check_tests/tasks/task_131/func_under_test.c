OS_SEC_L0_TEXT void OsTskReadyDel(struct TagTskCb *taskCb)
{
    struct TagOsRunQue *runQue = NULL;
    TSK_STATUS_CLEAR(taskCb, OS_TSK_READY);

    runQue = GET_RUNQ(taskCb->coreID);

    if(!(taskCb->isOnRq)) {
        return;
    }

    OsDequeueTask(runQue, taskCb, 0);
    OsDecNrRunning(runQue);

    if (taskCb == runQue->tskCurr) {
        OsReschedTask(taskCb);
    }

    return;
}
