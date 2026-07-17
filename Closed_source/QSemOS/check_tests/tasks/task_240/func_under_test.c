OS_SEC_ALW_INLINE INLINE bool OsTaskSpPcInfoReady(struct TagTskCb *taskCB)
{
    return ((((taskCB->taskStatus & OS_TSK_RUNNING) == 0) || (OS_INT_ACTIVE) || 
        (taskCB->coreID != THIS_CORE())) && OsTaskStackIsSave(taskCB));
}