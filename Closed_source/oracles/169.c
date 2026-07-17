OS_SEC_L2_TEXT U32 PRT_TimerStop(U32 mid, TimerHandle tmrHandle)
{
    U32 ret;
    U32 timerType;

    (void)mid;

    timerType = OS_TIMER_GET_TYPE(tmrHandle);
    if (timerType >= TIMER_TYPE_INVALID) {
        ret = OS_ERRNO_TIMER_HANDLE_INVALID;
        goto OS_TIMER_STOP_ERR;
    }

    if (g_timerApi[timerType].stopTimer == NULL) {
        ret = OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED;
        goto OS_TIMER_STOP_ERR;
    }

    ret = g_timerApi[timerType].stopTimer(tmrHandle);
    return ret;

OS_TIMER_STOP_ERR:
    OS_ERROR_LOG_REPORT(OS_ERR_LEVEL_HIGH, "timer stop error handle 0x%x\n", tmrHandle);
    return ret;
}
