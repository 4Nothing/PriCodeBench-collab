OS_SEC_ALW_INLINE INLINE U32 OsSwTmrCreateTimerParaChk(struct TimerCreatePara *createPara)
{
    /* OS接管中断, 中断处理函数必须为非NULL */
    if (createPara->callBackFunc == NULL) {
        return OS_ERRNO_TIMER_PROC_FUNC_NULL;
    }

    if (createPara->timerGroupId != OS_TICK_SWTMR_GROUP_ID) {
        return OS_ERRNO_TIMERGROUP_ID_INVALID;
    }

    if ((DIV64_REMAIN((U64)g_tickModInfo.tickPerSecond * (createPara->interval), OS_SYS_MS_PER_SECOND)) !=
        0) {  // ms转tick时，tick不为整数
        return OS_ERRNO_SWTMR_INTERVAL_NOT_SUITED;
    }

    if (createPara->interval >
        DIV64(OS_SYS_MS_PER_SECOND * ((U64)OS_MAX_U32), g_tickModInfo.tickPerSecond)) {  // 溢出判断
        return OS_ERRNO_SWTMR_INTERVAL_OVERFLOW;
    }
    return OS_OK;
}