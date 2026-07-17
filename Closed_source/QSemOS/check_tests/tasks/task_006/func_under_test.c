U32 OsPerfOutPutInit(void *buf, U32 size)
{
    U32 ret;
    bool releaseFlag = FALSE;

    if (PERF_BUFFER_WATERMARK_ONE_N == 0) {
        printf("perf buffer watermark configure failed\n");
        return OS_ERROR;
    }

    if (buf == NULL) {
        buf = PRT_MemAlloc(OS_MID_PERF, OS_MEM_DEFAULT_PT0, size);
        if (buf == NULL) {
            printf("perf malloc output buffer failed\n");
            return OS_ERROR;
        }

        releaseFlag = TRUE;
        (void)memset_s(buf, size, 0, size);
    }

    ret = PRT_RingbufInit(&g_perfOutputCb.ringbuf, buf, size);
    if (ret != OS_OK) {
        goto RELEASE;
    }

    g_perfOutputCb.waterMark = size / PERF_BUFFER_WATERMARK_ONE_N;
    g_perfBufNotifyHook = OsPerfDefaultNotify;
    return ret;
RELEASE:
    if (releaseFlag) {
        (void)PRT_MemFree(OS_MID_PERF, buf);
    }

    return ret;
}
