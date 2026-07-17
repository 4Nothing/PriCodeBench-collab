bool OsPerfParamValid(void)
{
    U32 index;
    U32 res = 0;
    U32 eventNum;
    PerfEvent *events = NULL;

    if (g_pmu == NULL) {
        return FALSE;
    }

    events = &g_pmu->events;
    eventNum = events->nr;
    for (index = 0; index < eventNum; index++) {
        res |= events->per[index].period;
    }

    if (res == 0) {
        return FALSE;
    }

    return TRUE;
}
