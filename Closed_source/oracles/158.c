OS_SEC_ALW_INLINE extern INLINE void OsResortSemList(struct TagTskCb *taskCb)
{  
    /* 调整信号量优先级队列，外部需要锁OsSemIfPrioLock，taskrq */
    struct TagTskCb *curTskCb = NULL;
    struct TagSemCb *semPended = (struct TagSemCb *)taskCb->taskPend;

    ListDelete(&taskCb->pendList);

    /* 遍历链表，找到合适的位置插入 */
    LIST_FOR_EACH(curTskCb, &semPended->semList, struct TagTskCb, pendList) {
        /* 找到一个优先级低于目标任务的任务 */
        if (taskCb->priority < curTskCb->priority) {
            /* 插入到该任务前面 */
            ListTailAdd(&taskCb->pendList, &curTskCb->pendList);
            return;
        }
    }

    /* 优先级低于或等于队列中所有其他任务，加在整个队列尾部 */
    ListTailAdd(&taskCb->pendList, &semPended->semList);
}