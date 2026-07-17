OS_SEC_L4_TEXT void OsFirstTimeSwitch(void)
{
    OsTskHighestSet();
    RUNNING_TASK = g_highestTask;
    TSK_STATUS_SET(RUNNING_TASK, OS_TSK_RUNNING);
#if defined(OS_OPTION_RR_SCHED)
    RUNNING_TASK->startTime = OsCurCycleGet64();
#endif
    OsTskContextLoad((uintptr_t)RUNNING_TASK);
    // never get here
    return;
}
