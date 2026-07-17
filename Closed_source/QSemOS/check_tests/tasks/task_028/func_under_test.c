OS_SEC_TEXT bool OsSwtmrTargetCheck(U32 coreId)
{
    (void)coreId;
    U32 idx;
    bool target = FALSE;
    struct TagSwTmrCtrl *swtmr = NULL;

    uintptr_t intSave = OsIntLock();
    if (g_swtmrCbArray != NULL) {
        for (idx = 0; idx < g_swTmrMaxNum; idx++) {
            swtmr = (struct TagSwTmrCtrl *)((struct TagSwTmrCtrl *)(g_swtmrCbArray) + idx);

            if ((swtmr->state == OS_TIMER_FREE) || (swtmr->state == OS_TIMER_CREATED)) {
                continue;
            } else {
                target = TRUE;
                break;
            }
        }
    }
    OsIntRestore(intSave);
    return target;
}