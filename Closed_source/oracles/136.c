static void OsPerfTimedHandle()
{
    U32 i;
    PerfRegs regs;
    Event *event = NULL;
    PerfEvent *events = &g_perfTimed.pmu.events;
    U32 eventNum = events->nr;

    (void)memset_s(&regs, sizeof(regs), 0, sizeof(regs));
    OsPerfFetchIrqRegs(&regs);

    for (i = 0; i < eventNum; i++) {
        event = &(events->per[i]);
        OsPerfUpdateEventCount(event, 1);
        OsPerfHandleOverFlow(event, &regs);
    }

    return;
}
