OS_SEC_L2_TEXT U32 PRT_TimerQuery(U32 mid, TimerHandle tmrHandle, U32 *expireTime)
{
    U32 ret;
    U32 timerType;
    (void)mid;

    // 检查指针是否为NULL
    if (expireTime == NULL) {
        return OS_ERRNO_TIMER_INPUT_PTR_NULL;
    }

    timerType = OS_TIMER_GET_TYPE(tmrHandle);
    if (timerType >= TIMER_TYPE_INVALID) {
        ret = OS_ERRNO_TIMER_HANDLE_INVALID;
        goto OS_TIMER_QUERY_ERR;
    }

    // 检查钩子是否为NULL
    if (g_timerApi[timerType].timerQuery == NULL) {
        ret = OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED;
        goto OS_TIMER_QUERY_ERR;
    }

    ret = g_timerApi[timerType].timerQuery(tmrHandle, expireTime);
    return ret;

OS_TIMER_QUERY_ERR:
    OS_ERROR_LOG_REPORT(OS_ERR_LEVEL_HIGH, "timer query error handle 0x%x\n", tmrHandle);
    return ret;
}
