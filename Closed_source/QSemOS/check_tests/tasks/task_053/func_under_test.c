U32 OsRwLockWrPend(pthread_rwlock_t *rwl, U32 timeout, U32 rwType)
{
    U32 ret;
    U32 intSave;
    struct TagTskCb *runTask;

    intSave = PRT_HwiLock();

    ret = OsRwLockCheck(rwl);
    if (ret != OS_OK) {
        PRT_HwiRestore(intSave);
        return ret;
    }

    runTask = (struct TagTskCb *)RUNNING_TASK;
    if (rwType == RWLOCK_TRYWR) {
        ret = OsRwlockTryWrCheck(runTask, rwl);
        if (ret != OS_OK) {
            PRT_HwiRestore(intSave);
            return ret;
        }
    }

    if (rwl->rw_count == 0) {
        rwl->rw_count = -1;
        rwl->rw_owner = (void *)runTask;
        PRT_HwiRestore(intSave);
        return OS_OK;
    }

    /* 如果读写锁被自身获取，只能获取一次. */
    if ((rwl->rw_count < 0) && ((struct TagTskCb *)(rwl->rw_owner) == runTask)) {
        if (rwl->rw_count == S32_MIN) {
            PRT_HwiRestore(intSave);
            return EINVAL;
        }
        PRT_HwiRestore(intSave);
        return OS_OK;
    }

    return OsRwLockPendSchedule(runTask, &(rwl->rw_write), timeout, intSave, rwl);
}
