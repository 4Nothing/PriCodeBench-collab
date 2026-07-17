void OsPerfOutPutInfo(void)
{
    printf("dump section data, addr: %p length: %#x\n", g_perfOutputCb.ringbuf.fifo, g_perfOutputCb.ringbuf.size);
    return;
}
