OS_SEC_L0_TEXT void OsPriorityInherit(struct TagSemCb *semPended, struct TagTskCb *runTsk)
{
    struct TagTskCb *semOwnerTask;
    struct TagSemCb *recurSem;
    TskPrior newPriority = runTsk->priority;
    
    if (GET_SEM_TYPE(semPended->semType) != SEM_TYPE_BIN) {
        return;
    }

    semOwnerTask = GET_TCB_HANDLE(semPended->semOwner);

    OsSpinLockTaskRq(semOwnerTask);
    while (semOwnerTask->priority > newPriority) {
        /* 信号量持有任务处于就绪状态，调整就绪队列后退出 */
        if (TSK_STATUS_TST(semOwnerTask, OS_TSK_READY)) {
            OsTskReadyDel(semOwnerTask);
            semOwnerTask->priority = newPriority;
            /* 添加到就绪链表同优先级尾部 */
            OsTskReadyAdd(semOwnerTask);
            OsSpinUnlockTaskRq(semOwnerTask);
            return;
        }
        semOwnerTask->priority = newPriority;
        /* 信号量持有任务不处于信号量阻塞状态与就绪状态，直接退出 */
        if (!TSK_STATUS_TST(semOwnerTask, OS_TSK_PEND)) {
            OsSpinUnlockTaskRq(semOwnerTask);
            return;
        }
        /* 信号量持有任务也处于阻塞状态，获取所阻塞的信号量 */
        recurSem = (struct TagSemCb *)semOwnerTask->taskPend;
        /* 如果是优先级唤醒模式先重排信号量的阻塞优先级队列 */
        if (recurSem->semMode == SEM_MODE_PRIOR) {
            OsResortSemList(semOwnerTask);
        }
        OsSpinUnlockTaskRq(semOwnerTask);
        /* 所阻塞的信号量不为互斥型，直接退出 */
        if (GET_SEM_TYPE(recurSem->semType) != SEM_TYPE_BIN) {
            return;
        }
        /* 所阻塞的信号量类型为互斥型，多级优先级继承，提升拥有该互斥信号量的任务优先级 */
        semOwnerTask = GET_TCB_HANDLE(recurSem->semOwner);
        OsSpinLockTaskRq(semOwnerTask);
    }
    OsSpinUnlockTaskRq(semOwnerTask);
}
