void OsPerfPrintCount(void)
{
    U32 index;
    U32 intSave;
    Event *event = NULL;
    U32 cpuid = PRT_GetCoreID();
    PerfEvent *events = &g_pmu->events;
    U32 eventNum = events->nr;

    // 主核在PRT_PerfStop时已关中断并拿到锁，这里只有从核才需要
    if (cpuid != g_primaryCoreId) {
        PERF_LOCK(intSave);
    }
    for (index = 0; index < eventNum; index++) {
        event = &(events->per[index]);

        /* filter out event counter with no event binded. */
        if (event->period == 0) {
            continue;
        }
#ifdef LOSCFG_SHELL_PERF
        printf("[%s] eventType: 0x%x [core %u]: %llu\n", g_pmu->getName(event), event->eventId, cpuid, event->count[cpuid]);
#else
        PRT_Printf("[%s] eventType: 0x%x [core %u]: %llu\n", g_pmu->getName(event), event->eventId, cpuid, event->count[cpuid]);
#endif
    }
    if (cpuid != g_primaryCoreId) {
        PERF_UNLOCK(intSave);
    }

    return;
}
