U32 OsRwLockRdPend(pthread_rwlock_t *rwl, U32 timeout, U32 rwType)
{
    U32 ret;
    U32 intSave;
    struct TagTskCb *runTask = NULL;

    intSave = PRT_HwiLock();

    ret = OsRwLockCheck(rwl);
    if (ret != OS_OK) {
        PRT_HwiRestore(intSave);
        return ret;
    }

    runTask = (struct TagTskCb *)RUNNING_TASK;

    if (rwType == RWLOCK_TRYRD) {
        ret = OsRwLockTryRdCheck(runTask, rwl);
        if (ret != OS_OK) {
            PRT_HwiRestore(intSave);
            return ret;
        }
    }

     /*
      * 读写锁为读模式或者初始化模式，并且当前读锁任务的优先级比第一个写锁pend任务的优先级高，
      * 当前读锁任务能获取锁
      */
    if (rwl->rw_count >= 0) {
        if (OsRwlockPriCompare(runTask, &(rwl->rw_write))) {
            if (rwl->rw_count == S32_MAX) {
                PRT_HwiRestore(intSave);
                return EINVAL;
            }
            rwl->rw_count++;
            PRT_HwiRestore(intSave);
            return OS_OK;
        }
    }

    if ((struct TagTskCb *)(rwl->rw_owner) == runTask) {
        PRT_HwiRestore(intSave);
        return EINVAL;
    }

    return OsRwLockPendSchedule(runTask, &(rwl->rw_read), timeout, intSave, rwl);
}
