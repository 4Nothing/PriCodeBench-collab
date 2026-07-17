OS_SEC_L2_TEXT U32 PRT_LogSetFilter(enum OsLogLevel level)
{
    int i;
    if (level < OS_LOG_EMERG || level > OS_LOG_NONE) {
        return -1;
    }
    for (i = 0; i < LOG_FACILITY_NUM; i++) {
        g_logFilter[i] = level;
    }
    return 0;
}
