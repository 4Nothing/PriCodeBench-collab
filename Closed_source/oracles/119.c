void PRT_PerfFlushHookReg(const PERF_BUF_FLUSH_HOOK func)
{
    U32 intSave;

    PERF_LOCK(intSave);
    OsPerfFlushHookReg(func);
    PERF_UNLOCK(intSave);

    return;
}
