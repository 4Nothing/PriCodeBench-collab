OS_SEC_L4_TEXT U32 OsSemInit(void)
{
    U32 ret = OsSemInitCb();

#if defined(OS_OPTION_SMP)
    OsSpinLockInitInner(&g_semPrioLock);
#endif

#if defined(OS_OPTION_SEM_PRIO_INHERIT)
    g_checkPrioritySet = OsCheckPrioritySet;
#endif
    return ret;
}
