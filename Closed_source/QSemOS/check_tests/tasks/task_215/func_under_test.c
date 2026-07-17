OS_SEC_ALW_INLINE INLINE void OsTaskIrqUnlock(uintptr_t intSave)
{
    struct TagOsRunQue *rq = THIS_RUNQ();

    if(LIKELY(rq->uniTaskLock == 1)) {
        rq->uniTaskLock = 0;

        if (UNLIKELY(rq->needReschedule) && UNLIKELY((rq->uniFlag & OS_HWI_ACTIVE_MASK) == 0)) {
            if ((OS_DI_STATE_CHECK(intSave)) != 0) {
                SMP_MC_SCHEDULE_TRIGGER(THIS_CORE());
                return;
            } else {
                OsTskScheduleFast();
            }
        }
        OsIntRestore(intSave);
        return;
    }
    if(rq->uniTaskLock > 1) {
        rq->uniTaskLock--;
        OsIntRestore(intSave);
        return;
    }
    OS_REPORT_ERROR(OS_ERRNO_TSK_UNLOCK_NO_LOCK);
    OsIntRestore(intSave);
}