U32 OsRwLockUnlock(pthread_rwlock_t *rwl, bool *needSched)
{
    struct TagTskCb *runTask;

    if (rwl == NULL) {
        return EINVAL;
    }
    if ((rwl->rw_magic & RWLOCK_COUNT_MASK) != RWLOCK_MAGIC_NUM) {
        return EINVAL;
    }

    if (rwl->rw_count == 0) {
        return EPERM;
    }

    runTask = RUNNING_TASK;
    if ((rwl->rw_count < 0) && ((struct TagTskCb *)(rwl->rw_owner) != runTask)) {
        return EPERM;
    }

    if (rwl->rw_count > 1) {
        rwl->rw_count--;
        return OS_OK;
    }

    if (rwl->rw_count < -1) {
        rwl->rw_count++;
        return OS_OK;
    }

    return OsRwLockPost(rwl, needSched);
}
