OS_SEC_L2_TEXT U32 PRT_LogInit(uintptr_t memBase)
{
    int ret;
    LOAD_FENCE();
    if (g_logMemBase != 0) {
        return 0;
    }
#if defined(OS_OPTION_SMP)
    ret = PRT_SplLockInit(&g_logLock);
    if (ret) {
        return -1;
    }
#endif
    g_logMemBase = memBase;
    STORE_FENCE();
    return 0;
}
