OS_SEC_L0_TEXT bool OsPriorityRestore(void)
{
    struct TagSemCb *curSem = NULL;
    struct TagTskCb *taskCb = NULL;
    struct TagTskCb *runTsk = RUNNING_TASK;
    TskPrior maxPriority = runTsk->origPriority;

    if (runTsk->priority == runTsk->origPriority) {
        return FALSE;
    }

    OsSpinLockTaskRq(runTsk);

    /* 遍历当前任务持有的互斥信号量，获取pend任务的最高优先级 */
    OsGetPendTskMaxPriority(runTsk, &maxPriority);

    /* 最高优先级与任务当前优先级相等，不需要调整 */
    if (maxPriority == runTsk->priority) {
        OsSpinUnlockTaskRq(runTsk);
        return FALSE;
    }

    /* 优先级降到最高优先，最高优先级至少为原始优先级 */
    OsTskReadyDel(runTsk);
    runTsk->priority = maxPriority;
    if (!TSK_STATUS_TST(runTsk, OS_TSK_SUSPEND_READY_BLOCK)) {
        // 中间放过锁，这里可能已经被置suspend, readAdd前必须有能否readyAdd的判断
        OsTskReadyAddBgd(runTsk);
    }
    OsSpinUnlockTaskRq(runTsk);
    return TRUE;
}
