OS_SEC_L2_TEXT U32 OsSetSysTimeHook(SysTimeFunc hook)
{
    /* 钩子不支持重复注册 */
    if (g_sysTimeHook != NULL) {
        return OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED;
    }
    g_sysTimeHook = hook;
    return OS_OK;
}
