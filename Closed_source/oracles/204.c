static OS_SEC_L2_TEXT uintptr_t OsLogLockOn()
{
    return PRT_SplIrqLock(&g_logLock);
}