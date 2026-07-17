void OsPerfNotifyHookReg(const PERF_BUF_NOTIFY_HOOK func)
{
    g_perfBufNotifyHook = func;
    return;
}
