OS_SEC_L4_TEXT bool OsSemBusy(SemHandle semHandle)
{
    struct TagSemCb *semCb = NULL;

    semCb = GET_SEM(semHandle);
    if (GET_MUTEX_TYPE(semCb->semType) != SEM_MUTEX_TYPE_RECUR && semCb->semCount == 0 &&
        GET_SEM_TYPE(semCb->semType) == SEM_TYPE_BIN) {
        return TRUE;
    } else if (GET_MUTEX_TYPE(semCb->semType) == SEM_MUTEX_TYPE_RECUR && semCb->semCount == 0 &&
        semCb->semOwner != RUNNING_TASK->taskPid) {
        return TRUE;
    }

    return FALSE;
}
