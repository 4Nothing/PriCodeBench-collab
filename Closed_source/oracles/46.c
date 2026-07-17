void OsRwLockPendPre(struct TagTskCb *runTask, struct TagListObject *list, U32 timeout, pthread_rwlock_t *rwl)
{
    OsTskReadyDel(runTask);

    TSK_STATUS_SET(runTask, OS_TSK_RW_PEND);
#if defined(OS_OPTION_SMP)
    runTask->taskPend = (void *)(&rwl->rwSpinLock);
#endif

    if (timeout != OS_WAIT_FOREVER) {
        TSK_STATUS_SET(runTask, OS_TSK_TIMEOUT);
        OsTskTimerAdd(runTask, timeout);
    }

    ListTailAdd(&(runTask->pendList), list);
}
