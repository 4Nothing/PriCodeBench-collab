void OsPerfOutPutFlush(void)
{
    if (g_perfBufFlushHook != NULL) {
        g_perfBufFlushHook(g_perfOutputCb.ringbuf.fifo, g_perfOutputCb.ringbuf.size);
    }

    return;
}
