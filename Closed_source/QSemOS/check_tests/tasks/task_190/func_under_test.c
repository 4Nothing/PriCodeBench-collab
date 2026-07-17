OS_SEC_ALW_INLINE INLINE void OsTskCreateTcbStatusSet(struct TagTskCb *taskCB, const struct TskInitParam *initParam)
{
    (void)initParam;
    OsSpinLockTaskRq(taskCB);
    taskCB->taskStatus = OS_TSK_SUSPEND | OS_TSK_INUSE;
    OsSpinUnlockTaskRq(taskCB);
    return;
}