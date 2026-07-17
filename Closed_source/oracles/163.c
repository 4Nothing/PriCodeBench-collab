static INLINE U32 OsSemPostErrorCheck(struct TagSemCb *semPosted, SemHandle semHandle)
{
    (void)semHandle;
    /* 检查信号量控制块是否UNUSED，排除大部分错误场景 */
    if (semPosted->semStat == OS_SEM_UNUSED) {
        return OS_ERRNO_SEM_INVALID;
    }
    if (GET_SEM_TYPE((semPosted)->semType) == SEM_TYPE_COUNT) {
        /* 释放计数型信号量且信号量计数大于最大计数 */
        if ((semPosted)->semCount >= OS_SEM_COUNT_MAX) {
            return OS_ERRNO_SEM_OVERFLOW;
        }
    } else if (GET_SEM_TYPE(semPosted->semType) == SEM_TYPE_BIN) {
        if (OS_INT_ACTIVE) {
            return OS_ERRNO_SEM_MUTEX_POST_INTERR;
        }

        /* 如果不是 互斥信号量的持有任务来做post操作 */
        if (semPosted->semOwner != RUNNING_TASK->taskPid) {
            return OS_ERRNO_SEM_MUTEX_NOT_OWNER_POST;
        }
    }
    return OS_OK;
}