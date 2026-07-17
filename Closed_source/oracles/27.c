OS_SEC_TEXT U64 OsSwtmrNearestTickGet(U32 coreId)
{
    (void)coreId;
    U32 idx;
    U64 ticks = OS_TICKLESS_FOREVER;
    struct TagSwTmrCtrl *swtmr = NULL;

    uintptr_t intSave = OsIntLock();
    if (g_swtmrCbArray != NULL) {
        for (idx = 0; idx < g_swTmrMaxNum; idx++) {
            swtmr = (struct TagSwTmrCtrl *)((struct TagSwTmrCtrl *)(g_swtmrCbArray) + idx);

            if ((swtmr->state == OS_TIMER_FREE) || (swtmr->state == OS_TIMER_CREATED)) {
                continue;
            }

            if ((g_uniTicks <= swtmr->expectedTick) && (swtmr->expectedTick < ticks)) {
                ticks = swtmr->expectedTick;
            }
        }
    }
    OsIntRestore(intSave);
    return ticks;
}