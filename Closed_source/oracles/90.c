static U32 OsRingbufReadLinear(Ringbuf *ringbuf, char *buf, U32 size)
{
    U32 ret;
    U32 cpSize;
    U32 remain;

    remain = ringbuf->endIdx - ringbuf->startIdx;
    cpSize = (remain < size) ? remain : size;

    if (cpSize == 0) {
        return 0;
    }

    ret = memcpy_s(buf, size, ringbuf->fifo + ringbuf->startIdx, cpSize);
    if (ret != OS_OK) {
        printf("read linear ring buffer failed, startIdx: %u, cpSize=%u\n", ringbuf->startIdx, cpSize);
        return 0;
    }

    ringbuf->remain += cpSize;
    ringbuf->startIdx += cpSize;

    return cpSize;
}
