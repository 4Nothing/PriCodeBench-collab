OS_SEC_L4_TEXT U32 OsSwTmrDeleteTimer(TimerHandle tmrHandle)
{
    struct TagSwTmrCtrl *swtmr = NULL;
#if defined(OS_OPTION_SMP)
    struct TagSwTmrCtrl swtmrTmp = {0};
#endif
    uintptr_t intSave;

    if (OS_TIMER_GET_INDEX(tmrHandle) >= g_swTmrMaxNum) {
        return OS_ERRNO_TIMER_HANDLE_INVALID;
    }

    // 转化为软件定时器内部ID号
    swtmr = g_swtmrCbArray + OS_SWTMR_ID_2_INDEX(tmrHandle);
    intSave = OsSwtmrIqrSplLock(swtmr);

    switch (swtmr->state & OS_SWTMR_STATUS_MASK) {
        case OS_TIMER_FREE:
            OsSwtmrIqrSplUnlock(swtmr, intSave);
            return OS_ERRNO_SWTMR_NOT_CREATED;
        case OS_TIMER_EXPIRED:
            swtmr->state = OS_SWTMR_PRE_FREE | (U8)OS_TIMER_EXPIRED;
            OsSwtmrIqrSplUnlock(swtmr, intSave);
            return OS_OK;
        case OS_TIMER_RUNNING:
            OsSwTmrStop(swtmr, TRUE);
            break;
        default:
            break;
    }
#if defined(OS_OPTION_SMP)
    swtmrTmp.coreID = swtmr->coreID;
#endif

    OsSwTmrDelete(swtmr);

#if defined(OS_OPTION_TICKLESS)
    OsSwtmrNearestTicksRefresh(&g_tmrSortLink);
#endif
    OsSwtmrIqrSplUnlock(&swtmrTmp, intSave);

    return OS_OK;
}
