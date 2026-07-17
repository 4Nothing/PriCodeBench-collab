OS_SEC_L2_TEXT U32 PRT_Log(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen)
{
    LOAD_FENCE();
    if (g_logMemBase == 0) {
        return -1;
    }
    if (OsCheckLog(level, facility) || str == NULL || strLen == 0) {
        return -1;
    }
    /* 检查是否被过滤 */
    if ((!g_logOn) || (level >= g_logFilter[facility - OS_LOG_F0])) {
        return 0;
    }

    if (strLen > (LOG_MAX_SIZE - 1)) {
        strLen = LOG_MAX_SIZE - 1;
    }
    return OsLog(level, facility, str, strLen);
}
