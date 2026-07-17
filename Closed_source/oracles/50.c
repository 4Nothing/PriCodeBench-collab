void OsRwLockTaskWake(struct TagTskCb *resumedTask)
{
    ListDelete(&resumedTask->pendList);

    if (TSK_STATUS_TST(resumedTask, OS_TSK_TIMEOUT)) {
        OS_TSK_DELAY_LOCKED_DETACH(resumedTask);
    }

    TSK_STATUS_CLEAR(resumedTask, OS_TSK_TIMEOUT | OS_TSK_RW_PEND);
#if defined(OS_OPTION_SMP)
    resumedTask->taskPend = NULL;
#endif

    if (!TSK_STATUS_TST(resumedTask, OS_TSK_SUSPEND_READY_BLOCK)) {
        OsTskReadyAddBgd(resumedTask);
    }
}
