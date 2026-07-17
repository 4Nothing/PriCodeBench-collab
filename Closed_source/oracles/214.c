OS_SEC_L2_TEXT U32 PRT_LogSetFilterByFacility(enum OsLogFacility facility, enum OsLogLevel level)
{
    if (level < OS_LOG_EMERG || level > OS_LOG_NONE || facility < OS_LOG_F0 || facility > OS_LOG_F7) {
        return -1;
    }
    g_logFilter[facility - OS_LOG_F0] = level;
    return 0;
}
