OS_SEC_L0_TEXT void OsMainSchedule(void)
{
    struct TagTskCb *prevTsk = RUNNING_TASK;
#if defined(OS_OPTION_RR_SCHED)
    U64 currTime = OsCurCycleGet64();
    OsTimeSliceUpdate(prevTsk, currTime, 0);
#endif
    if ((UNI_FLAG & OS_FLG_TSK_REQ) != 0) {
        /* 清除OS_FLG_TSK_REQ标记位 */
        UNI_FLAG &= ~OS_FLG_TSK_REQ;

        RUNNING_TASK->taskStatus &= ~OS_TSK_RUNNING;
        g_highestTask->taskStatus |= OS_TSK_RUNNING;

        RUNNING_TASK = g_highestTask;

        /* 有任务切换钩子&最高优先级任务等待调度 */
        if (prevTsk != g_highestTask) {
#if defined(OS_OPTION_RR_SCHED)
            g_highestTask->startTime = currTime;
#endif
            OsTskSwitchHookCaller(prevTsk->taskPid, g_highestTask->taskPid);
        }
    }
    // 如果中断没有驱动一个任务ready，直接回到被打断的任务
    OsTskContextLoad((uintptr_t)RUNNING_TASK);
}
