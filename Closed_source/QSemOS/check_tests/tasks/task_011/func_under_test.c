OS_SEC_ALW_INLINE INLINE void OsSemPrioBListUnLock(U32 flag)
{
#if defined(OS_OPTION_SEM_PRIO_INHERIT)
    if (flag) {
        OsSemPrioUnLock();
    }
#else
    (void)flag;
#endif
}