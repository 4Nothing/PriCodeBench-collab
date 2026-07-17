OS_SEC_TEXT void OsSetOfflineFlagHook(SetOfflineFlagFuncT setOfflineFlagFunc)
{
    g_setOfflineFlagHook = setOfflineFlagFunc;
}
