void OsPerfFlushHookReg(const PERF_BUF_FLUSH_HOOK func)
{
    g_perfBufFlushHook = func;
    return;
}
