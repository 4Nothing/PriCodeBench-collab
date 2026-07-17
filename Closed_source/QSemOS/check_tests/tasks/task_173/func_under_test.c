static inline bool OsQueuePendNeedProc(struct TagListObject *objectList)
{
    struct TagTskCb *resumedTask = NULL;

    /* 判断是否有任务阻塞于该队列 */
    if (ListEmpty(objectList)) {
        return FALSE;
    }

    /* 激活阻塞在该队列的首个任务 */
    resumedTask = GET_TCB_PEND(OS_LIST_FIRST(objectList));
    ListDelete(OS_LIST_FIRST(objectList));
    OsSpinLockTaskRq(resumedTask);

    /* 去除该任务的队列阻塞位 */
    TSK_STATUS_CLEAR(resumedTask, OS_TSK_QUEUE_PEND);
    resumedTask->taskPend = (void *)NULL;
    /* 如果阻塞的任务属于定时等待的任务时候，去掉其定时等待标志位，并将其从去除 */
    if ((resumedTask->taskStatus & OS_TSK_TIMEOUT) != 0) {
        /*
         * 添加PEND状态时，会加上TIMEOUT标志和timer，或者都不加。
         * 所以此时有TIMEOUT标志就一定有timer，且只有一个，且只用于该PEND方式
         */
        OS_TSK_DELAY_LOCKED_DETACH(resumedTask);
        TSK_STATUS_CLEAR(resumedTask, OS_TSK_TIMEOUT);
    }

    TSK_STATUS_SET(resumedTask, OS_TSK_QUEUE_BUSY);

    /* 如果去除队列阻塞位后，该任务不处于挂起态则将该任务挂入就绪队列并触发任务调度 */
    if ((resumedTask->taskStatus & OS_TSK_SUSPEND) == 0) {
        OsTskReadyAddBgd(resumedTask);
    }
    OsSpinUnlockTaskRq(resumedTask);
    return TRUE;
}