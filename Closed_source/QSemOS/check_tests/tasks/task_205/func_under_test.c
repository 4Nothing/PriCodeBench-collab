static OS_SEC_L2_TEXT void OsLogLockRestore(uintptr_t value)
{
    PRT_SplIrqUnlock(&g_logLock, value);
}