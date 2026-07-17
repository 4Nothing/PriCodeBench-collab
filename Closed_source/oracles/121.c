void PRT_PerfNotifyHookReg(const PERF_BUF_NOTIFY_HOOK func)
{
    U32 intSave;

    PERF_LOCK(intSave);
    OsPerfNotifyHookReg(func);
    PERF_UNLOCK(intSave);

    return;
}
