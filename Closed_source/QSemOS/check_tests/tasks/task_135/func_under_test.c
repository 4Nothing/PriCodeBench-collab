static U32 OsPerfTimedConfig()
{
    U32 i;
    U32 ret;
    U32 period;
    Event *event = NULL;
    PerfEvent *events = &g_perfTimed.pmu.events;
    U32 eventNum = events->nr;

    for (i = 0; i < eventNum; i++) {
        event = &(events->per[i]);
        period = event->period;
        if (event->eventId == PERF_COUNT_CPU_CLOCK) {
            if (period == 0) {
                printf("config period invalid, period:%u ms\n", period);
                return OS_ERRNO_TIMER_INTERVAL_INVALID;
            }

            struct TimerCreatePara timer = {
                .type = OS_TIMER_SOFTWARE,
                .mode = OS_TIMER_LOOP,
                .interval = period,
                .timerGroupId = 0,
                .callBackFunc = OsPerfSwtimer,
            };

            ret = PRT_TimerCreate(&timer, &g_swtPmuId);
            if (ret != OS_OK) {
                printf("perf timer create failed, ret = 0x%x\n", ret);
                return ret;
            }

            return OS_OK;
        }
    }

    return OS_ERROR;
}
