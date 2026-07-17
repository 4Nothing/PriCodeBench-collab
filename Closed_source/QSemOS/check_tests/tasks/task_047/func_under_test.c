U32 OsRwLockPendSchedule(struct TagTskCb *runTask, struct TagListObject *lockList, U32 timeout, U32 intSave,
    pthread_rwlock_t *rwl)
{
    struct TagListObject *node;

    if (timeout == 0) {
        PRT_HwiRestore(intSave);
        return EINVAL;
    }

    node = OsRwLockPendFindPos(runTask, lockList);
    OsRwLockPendPre(runTask, node, timeout, rwl);
    if (timeout != OS_WAIT_FOREVER) {
        OsTskSchedule();
        PRT_HwiRestore(intSave);
        intSave = PRT_HwiLock();
        /* 判断是否是等待信号量超时 */
        if (TSK_STATUS_TST(runTask, OS_TSK_TIMEOUT)) {
            TSK_STATUS_CLEAR(runTask, OS_TSK_TIMEOUT);
            PRT_HwiRestore(intSave);
            return ETIMEDOUT;
        }
        PRT_HwiRestore(intSave);
    } else {
        OsTskSchedule();
        PRT_HwiRestore(intSave);
    }

    return OS_OK;
}
