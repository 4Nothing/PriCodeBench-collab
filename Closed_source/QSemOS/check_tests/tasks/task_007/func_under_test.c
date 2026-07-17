U32 OsPerfOutPutRead(char *dest, U32 size)
{
    OsPerfOutPutFlush();
    return PRT_RingbufRead(&g_perfOutputCb.ringbuf, dest, size);
}
