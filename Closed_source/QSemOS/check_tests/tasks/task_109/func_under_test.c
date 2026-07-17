U32 OsPerfConfig(PerfEventConfig *eventsCfg)
{
    U32 i;
    U32 ret;
    U32 eventNum;

    g_pmu = OsPerfPmuGet(eventsCfg->type);
    if (g_pmu == NULL) {
        printf("perf config type invalid %u!\n", eventsCfg->type);
        return OS_ERRNO_PERF_INVALID_PMU;
    }

    eventNum = MIN(eventsCfg->eventsNr, PERF_MAX_EVENT);
    (void)memset_s(&g_pmu->events, sizeof(PerfEvent), 0, sizeof(PerfEvent));
    for (i = 0; i < eventNum; i++) {
        g_pmu->events.per[i].eventId = eventsCfg->events[i].eventId;
        g_pmu->events.per[i].period = eventsCfg->events[i].period;
    }
    g_pmu->events.nr = i;
    g_pmu->events.cntDivided = eventsCfg->predivided;
    g_pmu->type = eventsCfg->type;

    ret = g_pmu->config();
    if (ret != OS_OK) {
        printf("perf config failed, ret = 0x%x\n", ret);
        (void)memset_s(&g_pmu->events, sizeof(PerfEvent), 0, sizeof(PerfEvent));
        return OS_ERRNO_PERF_PMU_CONFIG_ERROR;
    }

    return OS_OK;
}
