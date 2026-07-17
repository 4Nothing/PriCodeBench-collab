OS_SEC_L2_TEXT U32 OsSwTmrQuery(TimerHandle tmrHandle, U32 *expireTime)
{
    struct TagSwTmrCtrl *swtmr = NULL;
    uintptr_t intSave;
    U32 remainTick;
    U32 remainMs;

    if (OS_TIMER_GET_INDEX(tmrHandle) >= g_swTmrMaxNum) {
        return OS_ERRNO_TIMER_HANDLE_INVALID;
    }

    // 转化为软件定时器内部ID
    swtmr = g_swtmrCbArray + OS_SWTMR_ID_2_INDEX(tmrHandle);
    intSave = OsSwtmrIqrSplLock(swtmr);

    if (swtmr->state == (U8)OS_TIMER_FREE) {
        OsSwtmrIqrSplUnlock(swtmr, intSave);
        return OS_ERRNO_SWTMR_NOT_CREATED;
    }

    /* 调用获取定时器剩余Tick数内部接口 */
    remainTick = OsSwTmrGetRemainTick(swtmr);
    remainMs = (U32)DIV64(((U64)remainTick * OS_SYS_MS_PER_SECOND), g_tickModInfo.tickPerSecond);
    if (DIV64_REMAIN((U64)remainTick * OS_SYS_MS_PER_SECOND, g_tickModInfo.tickPerSecond) != 0) {  // 若不整除，则+1
        remainMs++;
    }

    *expireTime = remainMs;

    OsSwtmrIqrSplUnlock(swtmr, intSave);
    return OS_OK;
}
