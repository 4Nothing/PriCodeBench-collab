OS_SEC_L4_TEXT U32 OsSwTimerGroupCreate(struct TmrGrpUserCfg *config, TimerGroupId *groupId)
{
    U32 ret;
    uintptr_t intSave;

    if (g_tickModInfo.tickPerSecond == 0) {
        return OS_ERRNO_TICK_NOT_INIT;
    }

    if (config->maxTimerNum == 0) {  // 最大定时器个数为零
        return OS_ERRNO_TIMER_NUM_ZERO;
    }

    if (config->maxTimerNum > (U32)OS_MAX_U16) {
        // 软件定时器个数为UINT16型，用户传入参数不能超过UINT16的最大值
        return OS_ERRNO_TIMER_NUM_TOO_LARGE;
    }

    intSave = OsIntLock();

    if (g_timerApi[TIMER_TYPE_SWTMR].createTimer != NULL) {  // 定时器组已经创建
        OsIntRestore(intSave);
        return OS_ERRNO_TIMER_TICKGROUP_CREATED;
    }

    g_swTmrMaxNum = config->maxTimerNum;

    ret = OsSwTmrResInit();
    if (ret != OS_OK) {
        OsIntRestore(intSave);
        return ret;
    }

    g_timerApi[TIMER_TYPE_SWTMR].createTimer = (TimerCreateFunc)OsSwTmrCreateTimer;
    g_timerApi[TIMER_TYPE_SWTMR].startTimer = (TimerStartFunc)OsSwTmrStartTimer;
    g_timerApi[TIMER_TYPE_SWTMR].stopTimer = (TimerStopFunc)OsSwTmrStopTimer;
    g_timerApi[TIMER_TYPE_SWTMR].deleteTimer = (TimerDeleteFunc)OsSwTmrDeleteTimer;
    g_timerApi[TIMER_TYPE_SWTMR].restartTimer = (TimerRestartFunc)OsSwTmrRestartTimer;
    g_timerApi[TIMER_TYPE_SWTMR].timerQuery = (TimerQueryFunc)OsSwTmrQuery;
    g_timerApi[TIMER_TYPE_SWTMR].getOverrun = (TimerGetOverrunFunc)OsSwTmrGetOverrun;

    *groupId = OS_TICK_SWTMR_GROUP_ID;

    OsIntRestore(intSave);
    return OS_OK;
}
