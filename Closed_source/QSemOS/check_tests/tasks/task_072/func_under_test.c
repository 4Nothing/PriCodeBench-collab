OS_SEC_L2_TEXT U32 OsSwTmrStopTimer(TimerHandle tmrHandle)
{
    struct TagSwTmrCtrl *swtmr = NULL;
    uintptr_t intSave;

    if (OS_TIMER_GET_INDEX(tmrHandle) >= g_swTmrMaxNum) {
        return OS_ERRNO_TIMER_HANDLE_INVALID;
    }

    swtmr = g_swtmrCbArray + OS_SWTMR_ID_2_INDEX(tmrHandle);
    intSave = OsSwtmrIqrSplLock(swtmr);

    switch (swtmr->state & OS_SWTMR_STATUS_MASK) {
        case OS_TIMER_FREE:
            OsSwtmrIqrSplUnlock(swtmr, intSave);
            return OS_ERRNO_SWTMR_NOT_CREATED;
        case OS_TIMER_EXPIRED:
            swtmr->state = OS_SWTMR_PRE_CREATED | (U8)OS_TIMER_EXPIRED;
            OsSwtmrIqrSplUnlock(swtmr, intSave);
            return OS_OK;
        case OS_TIMER_CREATED:
            OsSwtmrIqrSplUnlock(swtmr, intSave);
            return OS_ERRNO_SWTMR_UNSTART;
        default:
            break;
    }
    /*
     * 调用暂停定时器内部接口，在暂停时计算剩余时间
     */
    OsSwTmrStop(swtmr, FALSE);

#if defined(OS_OPTION_TICKLESS)
    OsSwtmrNearestTicksRefresh(CPU_SWTMR_SORT_LINK(swtmr->coreID));
#endif

    OsSwtmrIqrSplUnlock(swtmr, intSave);
    return OS_OK;
}
