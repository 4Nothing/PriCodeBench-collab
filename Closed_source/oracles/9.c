U32 OsPerfOutPutWrite(char *data, U32 size)
{
    if (!OsPerfOutPutBegin(size)) {
        return OS_ERROR;
    }

    PRT_RingbufWrite(&g_perfOutputCb.ringbuf, data, size);

    OsPerfOutPutEnd();
    return OS_OK;
}
