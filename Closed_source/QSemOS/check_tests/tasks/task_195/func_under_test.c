OS_SEC_L4_TEXT U32 OsTskRegister(struct TskModInfo *modInfo)
{
    if (((modInfo->maxNum) > (MAX_TASK_NUM)) || (modInfo->maxNum == 0)) {
        return OS_ERRNO_TSK_MAX_NUM_NOT_SUITED;
    }

    g_tskModInfo.maxNum = modInfo->maxNum;
    g_tskModInfo.defaultSize = modInfo->defaultSize;
    g_tskModInfo.idleStackSize = modInfo->idleStackSize;
    g_tskModInfo.magicWord = modInfo->magicWord;
    if (modInfo->timeSliceMs == 0) {
        g_tskModInfo.timeSliceMs = OS_TSK_DEFAULT_TIME_SLICE_MS;
    } else {
        g_tskModInfo.timeSliceMs = modInfo->timeSliceMs;
    }

    /* system clock 正常远大于1000 */
    g_timeSliceCycle = (g_systemClock / OS_SYS_MS_PER_SECOND) * g_tskModInfo.timeSliceMs;

    /* task模块有动态加载的场景 */
    if (g_tskIdleEntry == NULL) {
        g_tskIdleEntry = (TskEntryFunc)OsTskIdleBgd;
    }

    g_tskMaxNum = g_tskModInfo.maxNum;

    return OS_OK;
}
