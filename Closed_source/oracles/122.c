void PRT_PerfStart(U32 sectionId)
{
    U32 ret;
    U32 intSave;

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STOPED) {
        PRT_Printf("perf start status error : 0x%x\n", g_perfCb.status);
        goto PERF_START_ERROR;
    }

    if (OsPerfParamValid() == FALSE) {
        PRT_Printf("forgot call `PRT_Config(...)` before instrumenting?\n");
        goto PERF_START_ERROR;
    }

    if (g_perfCb.needStoreToBuffer) {
        ret = OsPerfHdrInit(sectionId);
        if (ret != OS_OK) {
            PRT_Printf("perf header init error 0x%x\n", ret);
            goto PERF_START_ERROR;
        }
    }

    /* send to all cpu to start pmu */
    SMP_CALL_PERF_FUNC(OsPerfStart);
    g_perfCb.status = PERF_STARTED;
    g_perfCb.startTime = OsCurCycleGet64();
PERF_START_ERROR:
    PERF_UNLOCK(intSave);

    return;
}
