U32 OsTimedPmuInit()
{
    U32 ret;

    g_perfTimed.pmu = (Pmu) {
        .type    = PERF_EVENT_TYPE_TIMED,
        .config  = OsPerfTimedConfig,
        .start   = OsPerfTimedStart,
        .stop    = OsPerfTimedStop,
        .getName = OsPerfGetEventName,
    };

    (void)memset_s(&g_perfTimed.pmu.events, sizeof(g_perfTimed.pmu.events), 0, sizeof(g_perfTimed.pmu.events));
    ret = OsPerfPmuRegister(&g_perfTimed.pmu);
    return ret;
}
