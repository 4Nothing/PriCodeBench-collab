static U32 OsPerfCollectData(Event *event, PerfSampleData *data, PerfRegs *regs)
{
    U32 ret;
    U32 size = 0;
    U32 depth;
    TskHandle threadId;
    U32 sampleType = g_perfCb.sampleType;
    char *p = (char *)data;

    if (sampleType & PERF_RECORD_CPU) {
        *(U32 *)(p + size) = PRT_GetCoreID();
        size += sizeof(data->cpuid);
    }

    if (sampleType & PERF_RECORD_TID) {
        struct TagTskCb *runTask = RUNNING_TASK;
        ret = PRT_TaskSelf(&threadId);
        if (ret != OS_OK) {
            PRT_Printf("when perf collect data, get task info failed, ret = 0x%x\n", ret);
            return;
        }
        *(U32 *)(p + size) = threadId;
        size += sizeof(data->taskId);
    }

    if (sampleType & PERF_RECORD_TYPE) {
        *(U32 *)(p + size) = event->eventId;
        size += sizeof(data->eventId);
    }

    if (sampleType & PERF_RECORD_PERIOD) {
        *(U32 *)(p + size) = event->period;
        size += sizeof(data->period);
    }

    if (sampleType & PERF_RECORD_TIMESTAMP) {
        *(U64 *)(p + size) = PRT_ClkGetCycleCount64();
        size += sizeof(data->time);
    }

    if (sampleType & PERF_RECORD_IP) {
        *(uintptr_t *)(p + size) = regs->pc;
        size += sizeof(data->pc);
    }

    if (sampleType & PERF_RECORD_CALLCHAIN) {
        depth = OsPerfBackTrace((uintptr_t *)(p + size + sizeof(data->callChain.ipNr)), PERF_MAX_CALLCHAIN_DEPTH, regs);
        *(U32 *)(p + size) = depth;
        size += sizeof(data->callChain.ipNr) + PERF_MAX_CALLCHAIN_DEPTH * sizeof(data->callChain.ip[0]);
    }

    return size;
}
