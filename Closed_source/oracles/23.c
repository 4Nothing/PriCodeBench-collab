OS_SEC_L4_TEXT void OsSwTmrCtrlInit(struct TagSwTmrCtrl *swtmrIn)
{
    U16 idx;
    struct TagSwTmrCtrl *temp = NULL;
    struct TagSwTmrCtrl *swtmr = swtmrIn;

    swtmr->swtmrIndex = 0;
    swtmr->state = (U8)OS_TIMER_FREE;
    temp = swtmr;
    swtmr++;
    for (idx = 1; idx < g_swTmrMaxNum; idx++, swtmr++) {
        swtmr->swtmrIndex = idx;
        swtmr->state = (U8)OS_TIMER_FREE;
        temp->next = swtmr;
        temp = swtmr;
    }

    g_swtmrScanHook = OsSwTmrScan;
}
