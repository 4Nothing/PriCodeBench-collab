OS_SEC_L0_TEXT void OsTskTimerAdd(struct TagTskCb *taskCb, uintptr_t timeout)
{
    struct TagTskCb *tskDelay = NULL;
    struct TagOsTskSortedDelayList *tskDlyBase = NULL;
    struct TagListObject *taskList = NULL;

    OS_SET_DLYBASE_AND_TSK_CORE(tskDlyBase, taskCb);

    taskCb->expirationTick = g_uniTicks + timeout;
    taskList = &tskDlyBase->tskList;

    CPU_OVERTIME_SORT_LIST_LOCK(tskDlyBase);

    if (ListEmpty(taskList)) {
        ListTailAdd(&taskCb->timerList, taskList);
    } else {
        /* Put the task to right location */
        LIST_FOR_EACH(tskDelay, taskList, struct TagTskCb, timerList) {
            if (tskDelay->expirationTick > taskCb->expirationTick) {
                break;
            }
        }

        ListTailAdd(&taskCb->timerList, &tskDelay->timerList);
    }
    OsTskDlyNearestTicksRefresh(tskDlyBase);

    CPU_OVERTIME_SORT_LIST_UNLOCK(tskDlyBase);

    return;
}