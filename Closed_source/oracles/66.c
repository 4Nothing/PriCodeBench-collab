OS_SEC_L4_TEXT U32 OsSwTmrGetRemain(struct TagSwTmrCtrl *swtmr)
{
    U32 retValue = 0;
    U32 temp;
    struct TagSwTmrCtrl *timer = NULL;
    struct TagListObject *listObject = NULL;

    temp = UWSORTINDEX(swtmr->idxRollNum);
    if (temp <= g_tmrSortLink.cursor) {
        temp = temp + OS_SWTMR_SORTLINK_LEN;
    }
    temp -= g_tmrSortLink.cursor;

    listObject = g_tmrSortLink.sortLink + (uintptr_t)UWSORTINDEX(swtmr->idxRollNum);

    timer = swtmr;

    while (timer != (struct TagSwTmrCtrl *)listObject) {
        retValue += UWROLLNUM(timer->idxRollNum);
        timer = timer->prev;
    }
    retValue = retValue * OS_SWTMR_SORTLINK_LEN;
    retValue = retValue + temp;

    return retValue;
}