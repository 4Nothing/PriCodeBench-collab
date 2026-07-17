OS_SEC_L0_TEXT U32 PRT_SemPost(SemHandle semHandle)
{
    U32 ret;
    uintptr_t intSave;
    struct TagSemCb *semPosted = NULL;

    if (semHandle >= (SemHandle)g_maxSem) {
        return OS_ERRNO_SEM_INVALID;
    }

    semPosted = GET_SEM(semHandle);
    SEM_CB_IRQ_LOCK(semPosted, intSave);

    ret = OsSemPostErrorCheck(semPosted, semHandle);
    if (ret != OS_OK) {
        SEM_CB_IRQ_UNLOCK(semPosted, intSave);
        return ret;
    }

    /* 信号量post无效，不需要调度 */
    if (OsSemPostIsInvalid(semPosted) == TRUE) {
        SEM_CB_IRQ_UNLOCK(semPosted, intSave);
        return OS_OK;
    }
    OsSemIfPrioLock(semPosted);

    /* 如果有任务阻塞在信号量上，就激活信号量阻塞队列上的首个任务 */
    if (!ListEmpty(&semPosted->semList)) {
        OsSemPendListGet(semPosted);
        OsSemIfPrioUnLock(semPosted);
        SEM_CB_UNLOCK(semPosted);
        /* 相当于快速切换+中断恢复 */
        OsTskScheduleFastPs(intSave);
    } else {
        semPosted->semCount++;
        
        semPosted->semOwner = OS_INVALID_OWNER_ID;
#if defined(OS_OPTION_BIN_SEM)
        /* 如果释放的是互斥信号量，就从释放此互斥信号量任务的持有链表上摘除它 */
        if (GET_SEM_TYPE(semPosted->semType) == SEM_TYPE_BIN) {
            ListDelete(&semPosted->semBList);
#if defined(OS_OPTION_SEM_PRIO_INHERIT)
            /* 尝试降低当前任务优先级 */
            if (OsPriorityRestore()) {
                /* 优先级发生变化，触发任务调度 */
                OsSemIfPrioUnLock(semPosted);
                SEM_CB_UNLOCK(semPosted);
                OsTskSchedule();
                OsIntRestore(intSave);
                return OS_OK;
            }
#endif
        }
#endif
        OsSemIfPrioUnLock(semPosted);
        SEM_CB_UNLOCK(semPosted);
    }

    OsIntRestore(intSave);
    return OS_OK;
}
