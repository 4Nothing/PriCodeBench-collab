void OsGetPendTskMaxPriority(struct TagTskCb *taskCb, TskPrior *maxPriority)
{
    TskPrior curMaxPrior = *maxPriority;
    struct TagSemCb *curSem = NULL;
    struct TagTskCb *pendTask = NULL;

    LIST_FOR_EACH(curSem, &taskCb->semBList, struct TagSemCb, semBList) {
        if (!ListEmpty(&curSem->semList)) {
            pendTask = GET_TCB_PEND(OS_LIST_FIRST(&(curSem->semList)));
            if (pendTask->priority < curMaxPrior) {
                curMaxPrior = pendTask->priority;
            }
        }
    }
    *maxPriority = curMaxPrior;
    return;
}