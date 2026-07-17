U32 OsRwLockPost(pthread_rwlock_t *rwl, bool *needSched)
{
    U32 rwlockMode;
    struct TagTskCb *resumedTask = NULL;
    U32 pendedWriteTaskPri = 0;

    rwl->rw_count = 0;
    rwl->rw_owner = NULL;
    rwlockMode = OsRwLockGetMode(&(rwl->rw_read), &(rwl->rw_write));
    if (rwlockMode == RWLOCK_NONE_MODE) {
        return OS_OK;
    }

    /* 唤醒第一个被pend的写锁任务 */
    if ((rwlockMode == RWLOCK_WRITE_MODE) || (rwlockMode == RWLOCK_WRITEFIRST_MODE)) {
        resumedTask = GET_TCB_PEND(OS_LIST_FIRST(&(rwl->rw_write)));
        rwl->rw_count = -1;
        rwl->rw_owner = (void *)resumedTask;
        OsRwLockTaskWake(resumedTask);
        if (needSched != NULL) {
            *needSched = TRUE;
        }
        return OS_OK;
    }

    /* 唤醒被pend的读锁任务 */
    if (rwlockMode == RWLOCK_READFIRST_MODE) {
        pendedWriteTaskPri = GET_TCB_PEND(OS_LIST_FIRST(&(rwl->rw_write)))->priority;
    }

    resumedTask = GET_TCB_PEND(OS_LIST_FIRST(&(rwl->rw_read)));
    rwl->rw_count = 1;
    OsRwLockTaskWake(resumedTask);
    while (!ListEmpty(&(rwl->rw_read))) {
        resumedTask = GET_TCB_PEND(OS_LIST_FIRST(&(rwl->rw_read)));
        if ((rwlockMode == RWLOCK_READFIRST_MODE) && (resumedTask->priority >= pendedWriteTaskPri)) {
            break;
        }

        if (rwl->rw_count == S32_MAX) {
            return EINVAL;
        }

        rwl->rw_count++;
        OsRwLockTaskWake(resumedTask);
    }

    if (needSched != NULL) {
        *needSched = TRUE;
    }
    return OS_OK;
}
