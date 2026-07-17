static U32 OsPerfTimedStart()
{
    U32 i;
    U32 ret;
    Event *event = NULL;
    U32 cpuid = PRT_GetCoreID();
    PerfEvent *events = &g_perfTimed.pmu.events;
    U32 eventNum = events->nr;

    for (i = 0; i < eventNum; i++) {
        event = &(events->per[i]);
        event->count[cpuid] = 0;
    }

    if (cpuid != g_primaryCoreId) {
        return OS_OK;
    }

    ret = PRT_TimerStart(0, g_swtPmuId);
    if (ret != OS_OK) {
        (void)PRT_TimerDelete(0, g_swtPmuId);
        printf("perf timer start failed, ret = 0x%x\n", ret);
        return ret;
    }

    return OS_OK;
}
