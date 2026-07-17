OS_SEC_ALW_INLINE INLINE U32 OsSemPrioBListLock(struct TagTskCb *taskCb)
{
#if defined(OS_OPTION_SEM_PRIO_INHERIT)
    if (!ListEmpty(&taskCb->semBList)) {
        OsSemPrioLock();
        return 1;
    }
#endif
    return 0;
}