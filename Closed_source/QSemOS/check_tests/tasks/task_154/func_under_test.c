OS_SEC_L4_TEXT U32 OsCheckPrioritySet(struct TagTskCb *taskCb, TskPrior taskPrio)
{
    struct TagSemCb *curSem = NULL;
    TskPrior maxPriority = taskCb->origPriority;

    if (taskCb->origPriority != taskCb->priority) {
        return OS_ERRNO_TSK_PRIORITY_INHERIT;
    }

    curSem = taskCb->taskPend;
    if (TSK_STATUS_TST(taskCb, OS_TSK_PEND) && curSem != NULL) {
        if (GET_SEM_TYPE(curSem->semType) == SEM_TYPE_BIN) {
            return OS_ERRNO_TSK_PEND_MUTEX;
        }
        if (curSem->semMode == SEM_MODE_PRIOR) {
            // 只有开启优先级继承时才有该限制，保证唤醒的任务在等待队列中优先级最高，如此不需要触发优先级继承
            return OS_ERRNO_TSK_PEND_PRIOR;
        }
    }

    /* 未持有互斥信号量 */
    if (ListEmpty(&taskCb->semBList)) {
        return OS_OK;
    }

    /* 遍历持有的互斥信号量，获取pend任务的最高优先级，此处需要semIfPrior锁保护 */
    OsGetPendTskMaxPriority(taskCb, &maxPriority);

    if (taskPrio > maxPriority) {
        return OS_ERRNO_TSK_PRIOR_LOWER_THAN_PENDTSK;
    }

    return OS_OK;
}
