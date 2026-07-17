OS_SEC_L2_TEXT U32 PRT_TimerRestart(U32 mid, TimerHandle tmrHandle)
{
    U32 ret;
    U32 timerType;
    (void)mid;

    timerType = OS_TIMER_GET_TYPE(tmrHandle);
    if (timerType >= TIMER_TYPE_INVALID) {
        ret = OS_ERRNO_TIMER_HANDLE_INVALID;
        goto OS_TIMER_RESTART_ERR;
    }

    if (g_timerApi[timerType].restartTimer == NULL) {
        ret = OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED;
        goto OS_TIMER_RESTART_ERR;
    }

    ret = g_timerApi[timerType].restartTimer(tmrHandle);
    return ret;

OS_TIMER_RESTART_ERR:
    OS_ERROR_LOG_REPORT(OS_ERR_LEVEL_HIGH, "timer restart error handle 0x%x\n", tmrHandle);
    return ret;
}
