struct TagListObject *OsRwLockPendFindPos(struct TagTskCb *runTask, struct TagListObject *lockList)
{
    struct TagListObject *node = NULL;
    struct TagTskCb *taskFirst;
    struct TagTskCb *taskLast;

    if (ListEmpty(lockList)) {
        node = lockList;
    } else {
        taskFirst = GET_TCB_PEND(OS_LIST_FIRST(lockList));
        taskLast = GET_TCB_PEND(LIST_LAST(lockList));
        if (taskFirst->priority > runTask->priority) {
            node = lockList->next;
        } else if (taskLast->priority <= runTask->priority) {
            node = lockList;
        } else {
            node = OsRwLockPendFindPosSub(runTask, lockList);
        }
    }

    return node;
}
