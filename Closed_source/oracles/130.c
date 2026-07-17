OS_SEC_ALW_INLINE INLINE void OsTskReadyAddOnly(struct TagTskCb *task)
{
    struct TagOsRunQue *rq = GET_RUNQ(task->coreID);
    TSK_STATUS_SET(task, OS_TSK_READY);
#if defined(OS_OPTION_RR_SCHED)
    task->timeSlice = g_timeSliceCycle;
#endif
    if(UNLIKELY(task->isOnRq)){
        return;
    }
    OsEnqueueTask(rq, task, 0);

    OsIncNrRunning(rq);

    return;
}