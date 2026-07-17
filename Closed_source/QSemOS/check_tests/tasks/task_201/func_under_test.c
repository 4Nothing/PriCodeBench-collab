OS_SEC_ALW_INLINE INLINE U32 OsCheckLog(enum OsLogLevel level, enum OsLogFacility facility)
{
    if (level < OS_LOG_EMERG || level > OS_LOG_DEBUG || facility < OS_LOG_F0 || facility > OS_LOG_F7) {
        return -1;
    }
    return 0;
}