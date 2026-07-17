OS_SEC_L2_TEXT U32 OsSwTmrGetOverrun(TimerHandle tmrHandle, U32 *overrun)
{
    struct TagSwTmrCtrl *swtmr = NULL;
    uintptr_t intSave;

    if (OS_TIMER_GET_INDEX(tmrHandle) >= g_swTmrMaxNum) {
        return OS_ERRNO_TIMER_HANDLE_INVALID;
    }

    // 转化为软件定时器内部ID号
    swtmr = g_swtmrCbArray + OS_SWTMR_ID_2_INDEX(tmrHandle);
    intSave = OsSwtmrIqrSplLock(swtmr);

    *overrun = (U32)swtmr->overrun;

    OsSwtmrIqrSplUnlock(swtmr, intSave);

    return OS_OK;
}
