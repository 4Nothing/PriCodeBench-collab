static U32 OsRingbufWriteLinear(Ringbuf *ringbuf, const char *buf, U32 size)
{
    U32 cpSize;
    U32 ret;

    cpSize = (ringbuf->remain < size) ? ringbuf->remain : size;

    if (cpSize == 0) {
        return 0;
    }

    ret = memcpy_s(ringbuf->fifo + ringbuf->endIdx, ringbuf->remain, buf, cpSize);
    if (ret != OS_OK) {
        printf("write linear ring buffer failed, endIdx: %u, cpSize=%u\n", ringbuf->endIdx, cpSize);
        return 0;
    }

    ringbuf->remain -= cpSize;
    ringbuf->endIdx += cpSize;

    return cpSize;
}
