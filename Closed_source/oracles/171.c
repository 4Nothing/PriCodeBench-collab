OS_SEC_ALW_INLINE INLINE U32 OsInnerPend(U16 *count, struct TagListObject *pendList, U32 timeOut,
                                         struct TagQueCb *queueCb)
{
    struct TagTskCb *runTsk = NULL;

    /* 判断是否需要阻塞 */
    if (*count > 0) {
        (*count)--;
        return OS_OK;
    }

    /* 阻塞任务 */
    if (timeOut == OS_QUEUE_NO_WAIT) {
        return OS_ERRNO_QUEUE_NO_SOURCE;
    }

    if (OS_INT_ACTIVE) {
        return OS_ERRNO_QUEUE_IN_INTERRUPT;
    }

    /* 如果锁任务的情况下 */
    if (OS_TASK_LOCK_DATA != 0) {
        return OS_ERRNO_QUEUE_PEND_IN_LOCK;
    }

    /* 利用局部变量 runTsk 完成对任务控制块的相关操作，不修改 RUNNING_TASK */
    runTsk = (struct TagTskCb *)RUNNING_TASK;

    /* 从任务的Ready list上把当前任务删除，添加到pend list上 */
    OsSpinLockTaskRq(runTsk);
    OsTskReadyDel(runTsk);

    TSK_STATUS_SET(runTsk, OS_TSK_QUEUE_PEND);
    runTsk->taskPend = (void *)queueCb;
    ListTailAdd(&runTsk->pendList, pendList);

    /* 如果timeOut > 0,timeOut为等待时间，如果timeOut == OS_QUEUE_WAIT_FOREVER，表示永久等待 */
    if (timeOut != OS_QUEUE_WAIT_FOREVER) {
        /* 如果不是永久等待则将任务挂到计时器链表中，设置OS_TSK_TIMEOUT是为了判断是否等待超时 */
        TSK_STATUS_SET(runTsk, OS_TSK_TIMEOUT);
        OsTskTimerAdd(runTsk, timeOut);
    }

    OsSpinUnlockTaskRq(runTsk);

    /* 调用函数之前已经关中断，此处关中断进行调度 */
    /* 触发任务调度 */
    QUEUE_CB_UNLOCK(queueCb);
    OsTskSchedule();
    QUEUE_CB_LOCK(queueCb);

    OsSpinLockTaskRq(runTsk);
    TSK_STATUS_CLEAR(runTsk, OS_TSK_QUEUE_BUSY);

    /* 判断是否是等待队列超时 */
    if ((runTsk->taskStatus & OS_TSK_TIMEOUT) != 0) {
        TSK_STATUS_CLEAR(runTsk, OS_TSK_TIMEOUT);
        OsSpinUnlockTaskRq(runTsk);

        /* 在函数的外面会开中断 */
        return OS_ERRNO_QUEUE_TIMEOUT;
    }
    OsSpinUnlockTaskRq(runTsk);
    /* 在函数的外面会开中断 */
    return OS_OK;
}