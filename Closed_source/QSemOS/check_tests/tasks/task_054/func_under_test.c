OS_SEC_ALW_INLINE INLINE bool OsRwlockPriCompare(struct TagTskCb *runTask, struct TagListObject *rwList)
{
    struct TagTskCb *task;

    if (!ListEmpty(rwList)) {
        task = GET_TCB_PEND(OS_LIST_FIRST(rwList));
        if (runTask->priority < task->priority) {
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}