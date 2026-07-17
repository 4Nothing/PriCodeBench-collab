OS_SEC_L4_TEXT uintptr_t OsSwtmrIqrSplLock(struct TagSwTmrCtrl *swtmr)
{
    uintptr_t intSave = OsIntLock();
    struct TagSwTmrSortLinkAttr *tmrSort = NULL;
    U32 preCoreID;

    while (1)
    {
        preCoreID = swtmr->coreID;
        tmrSort = CPU_SWTMR_SORT_LINK(swtmr->coreID);

        OsSplLock(&tmrSort->spinLock);

        if(preCoreID == swtmr->coreID) {
            break;
        } else {
            OsSplUnlock(&tmrSort->spinLock);
        }
    }
    return intSave;
    
}
