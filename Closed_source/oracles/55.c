U32 OsRwlockTryWrCheck(struct TagTskCb *runTask, pthread_rwlock_t *rwl)
{
    /* 读写锁被读锁获得，当前写锁任务不能获取锁 */
    if (rwl->rw_count > 0) {
        return EBUSY;
    }

    /* 读写锁被写锁获得，当前写锁任务不能获取锁. */
    if ((rwl->rw_count < 0) && ((struct TagTskCb *)(rwl->rw_owner) != runTask)) {
        return EBUSY;
    }

    return OS_OK;
}
