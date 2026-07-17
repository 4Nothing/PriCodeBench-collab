OS_SEC_L4_TEXT uintptr_t OsSwtmrIqrSplUnlock(struct TagSwTmrCtrl *swtmr, uintptr_t intSave)
{
    struct TagSwTmrSortLinkAttr *tmrSort = NULL;
    tmrSort = CPU_SWTMR_SORT_LINK(swtmr->coreID);
    OsSplUnlock(&tmrSort->spinLock);
    OsIntRestore(intSave);
}
