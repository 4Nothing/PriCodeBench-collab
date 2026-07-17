OS_SEC_L4_TEXT U32 PRT_TimerCreate(struct TimerCreatePara *createPara, TimerHandle *tmrHandle)
{
    U32 ret;
    U32 timerType = TIMER_TYPE_SWTMR;

    if ((createPara == NULL) || (tmrHandle == NULL)) {
        return OS_ERRNO_TIMER_INPUT_PTR_NULL;
    }

    if (createPara->interval == 0) {
        return OS_ERRNO_TIMER_INTERVAL_INVALID;
    }

    if ((createPara->mode != OS_TIMER_LOOP) && (createPara->mode != OS_TIMER_ONCE)) {
        return OS_ERRNO_TIMER_MODE_INVALID;
    }

    switch (createPara->type) {
        case OS_TIMER_HARDWARE:
            timerType = TIMER_TYPE_HWTMR;
            break;

        case OS_TIMER_SOFTWARE:
            timerType = TIMER_TYPE_SWTMR;
            break;

        default:
            ret = OS_ERRNO_TIMER_TYPE_INVALID;
            goto OS_TIMER_CREATE_ERR;
    }

    if (g_timerApi[timerType].createTimer == NULL) {
        ret = OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED;
        goto OS_TIMER_CREATE_ERR;
    }

    ret = g_timerApi[timerType].createTimer(createPara, tmrHandle);
    return ret;

OS_TIMER_CREATE_ERR:
    OS_ERROR_LOG_REPORT(OS_ERR_LEVEL_HIGH, "timer create error, timer type is 0x%x\n", createPara->type);
    return ret;
}
