OS_SEC_L0_TEXT void OsSemPendListPut(struct TagSemCb *semPended, U32 timeOut)
{
    struct TagTskCb *curTskCb = NULL;
    struct TagTskCb *runTsk = RUNNING_TASK;
    struct TagListObject *pendObj = &runTsk->pendList;
    struct TagOsRunQue *runQue;

    runQue = OsSpinLockRunTaskRq();
    OsTskReadyDel((struct TagTskCb *)runTsk);

    runTsk->taskPend = (void *)semPended;

    TSK_STATUS_SET(runTsk, OS_TSK_PEND);

#if defined(OS_OPTION_SEM_PRIOR)
    /* 根据唤醒方式挂接此链表，同优先级再按FIFO子顺序插入 */
    if (semPended->semMode == SEM_MODE_PRIOR) {
        LIST_FOR_EACH(curTskCb, &semPended->semList, struct TagTskCb, pendList) {
            if (curTskCb->priority > runTsk->priority) {
                ListTailAdd(pendObj, &curTskCb->pendList);
                goto TIMER_ADD;
            }
        }
    }
#endif
    /* 如果到这里，说明是FIFO方式；或者是优先级方式且挂接首个节点或者挂接尾节点 */
    ListTailAdd(pendObj, &semPended->semList);
TIMER_ADD:
    // timer超时链表添加
    if (timeOut != OS_WAIT_FOREVER) {
        /* 如果不是永久等待则将任务挂到计时器链表中，设置OS_TSK_TIMEOUT是为了判断是否等待超时 */
        TSK_STATUS_SET(runTsk, OS_TSK_TIMEOUT);

        OsTskTimerAdd((struct TagTskCb *)runTsk, timeOut);
    }

    OsSpinUnLockRunTaskRq(runQue);

#if defined(OS_OPTION_SEM_PRIO_INHERIT)
    /* 优先级继承，仅二进制信号量支持 */
    OsPriorityInherit(semPended, runTsk);
#endif
}
