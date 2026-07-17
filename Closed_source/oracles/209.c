extern U32 PRT_LogFormat(enum OsLogLevel level, enum OsLogFacility facility, const char *fmt, ...)
{
    int len;
    va_list vaList;
    char buff[BUFFER_BLOCK_SIZE];

    LOAD_FENCE();
    if (g_logMemBase == 0) {
        return -1;
    }
    if (OsCheckLog(level, facility) || fmt == NULL) {
        return -1;
    }

    /* 检查是否被过滤 */
    if ((!g_logOn) || (level >= g_logFilter[facility - OS_LOG_F0])) {
        return 0;
    }

    memset_s(buff, BUFFER_BLOCK_SIZE, 0, BUFFER_BLOCK_SIZE);
    va_start(vaList, fmt);
    // 字符串格式化由用户负责
    len = vsnprintf_s(buff, BUFFER_BLOCK_SIZE, BUFFER_BLOCK_SIZE, fmt, vaList);
    va_end(vaList);
    if (len < 0) {
        return len;
    }

    if (len > (LOG_MAX_SIZE - 1)) {
        len = LOG_MAX_SIZE - 1;
    }
    return OsLog(level, facility, buff, len);
}
