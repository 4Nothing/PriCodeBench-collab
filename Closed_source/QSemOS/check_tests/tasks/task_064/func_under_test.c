OS_SEC_L2_TEXT void OsSwTmrDelete(struct TagSwTmrCtrl *swtmr)
{
    swtmr->next = g_tmrFreeList;
    g_tmrFreeList = swtmr;
    swtmr->state = (U8)OS_TIMER_FREE;
}