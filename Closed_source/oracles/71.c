OS_SEC_L2_TEXT void OsSwTmrStop(struct TagSwTmrCtrl *swtmr, bool reckonOff)
{
#if !defined(OS_OPTION_SMP)
    U32 idx;
    struct TagListObject *listObject = NULL;
    uintptr_t intSave;

    intSave = OsIntLock();
    /* if the stoping node have next node ,set its rollnum to the next node's */
    idx = UWSORTINDEX(swtmr->idxRollNum);
    listObject = (g_tmrSortLink.sortLink + idx);
    if (swtmr->next != (struct TagSwTmrCtrl *)listObject) {
        UWROLLNUMADD(swtmr->next->idxRollNum, swtmr->idxRollNum);
    }
#endif

    if (!reckonOff) {
        /* 调用获取定时器剩余Tick数内部接口 */
        swtmr->idxRollNum = OsSwTmrGetRemainTick(swtmr);
    }

    swtmr->next->prev = swtmr->prev;
    swtmr->prev->next = swtmr->next;
    /* 定时器被暂停，修改定时器状态 */
    swtmr->state = (U8)OS_TIMER_CREATED;
    swtmr->next = NULL;
    swtmr->prev = NULL;

#if !defined(OS_OPTION_SMP)
    OsIntRestore(intSave);
#endif
}
