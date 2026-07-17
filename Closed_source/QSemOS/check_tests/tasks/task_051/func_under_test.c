U32 OsRwLockTryRdCheck(struct TagTskCb *runTask, pthread_rwlock_t *rwl)
{
    if ((struct TagTskCb *)(rwl->rw_owner) == runTask) {
        return EINVAL;
    }

    /* 读写锁为读模式或者初始化模式，并且当前读锁任务的优先级比第一个写锁pend任务的优先级低，
     * 当前读锁任务不能获取锁
     */
    if ((rwl->rw_count >= 0) && !OsRwlockPriCompare(runTask, &(rwl->rw_write))) {
        return EBUSY;
    }

    /* 读写锁被写锁获得，当前读锁任务不能获取锁 */
    if (rwl->rw_count < 0) {
        return EBUSY;
    }

    return OS_OK;
}
