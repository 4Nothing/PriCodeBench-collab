OS_SEC_L4_TEXT U32 OsSwTmrGetRemainTick(struct TagSwTmrCtrl *swtmr)
{
    U32 temp;
    uintptr_t intSave;

    intSave = OsIntLock();
    switch (swtmr->state & OS_SWTMR_STATUS_MASK) {
        case OS_TIMER_CREATED:
            OsIntRestore(intSave);
            return (swtmr->idxRollNum);
        case OS_TIMER_EXPIRED:
            OsIntRestore(intSave);
            return 0;
        default:
            break;
    }

    temp = OsSwTmrGetRemain(swtmr);

    OsIntRestore(intSave);
    return temp;
}
