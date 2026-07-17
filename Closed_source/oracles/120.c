U32 PRT_PerfInit(void *buf, U32 size)
{
    U32 ret;
    U32 intSave;

#if defined(OS_OPTION_SMP)
    ret = PRT_SplLockInit(&g_perfSpin);
    if (ret != OS_OK) {
        PRT_Printf("perf spin lock init failed, ret = 0x%x\n", ret);
        return OS_ERROR;
    }
#endif

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_UNINIT) {
        ret = OS_ERRNO_PERF_STATUS_INVALID;
        goto PERF_INIT_ERROR;
    }

    ret = OsPmuInit();
    if (ret != OS_OK) {
        goto PERF_INIT_ERROR;
    }

    ret = OsPerfOutPutInit(buf, size);
    if (ret != OS_OK) {
        ret = OS_ERRNO_PERF_BUF_ERROR;
        goto PERF_INIT_ERROR;
    }
    g_perfCb.status = PERF_STOPED;
PERF_INIT_ERROR:
    PERF_UNLOCK(intSave);

    return ret;
}
