U32 PRT_PerfConfig(PerfConfigAttr *attr)
{
    U32 ret;
    U32 intSave;

    if (attr == NULL) {
        return OS_ERRNO_PERF_CONFIG_NULL;
    }

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STOPED) {
        ret = OS_ERRNO_PERF_STATUS_INVALID;
        PRT_Printf("perf config status error : 0x%x\n", g_perfCb.status);
        goto PERF_CONFIG_ERROR;
    }

    g_pmu = NULL;

    g_perfCb.needStoreToBuffer = attr->needStoreToBuffer;
    g_perfCb.taskFilterEnable = attr->taskFilterEnable;
    g_perfCb.sampleType = attr->sampleType;

    if (attr->taskFilterEnable) {
        ret = memcpy_s(g_perfCb.taskIds, PERF_MAX_FILTER_TSKS * sizeof(U32), attr->taskIds,
                       attr->taskIdsNr * sizeof(U32));
        if (ret != OS_OK) {
            PRT_Printf("perf copy filter task failed, ret = 0x%x\n", ret);
            goto PERF_CONFIG_ERROR;
        }
        g_perfCb.taskIdsNr = MIN(attr->taskIdsNr, PERF_MAX_FILTER_TSKS);
    }

    ret = OsPerfConfig(&attr->eventsCfg);

PERF_CONFIG_ERROR:
    PERF_UNLOCK(intSave);

    return ret;
}
