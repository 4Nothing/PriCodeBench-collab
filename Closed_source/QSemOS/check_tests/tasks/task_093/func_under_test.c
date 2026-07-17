static U32 OsRingbufWriteLoop(Ringbuf *ringbuf, const char *buf, U32 size)
{
    U32 ret;
    U32 right;
    U32 cpSize;

    right = ringbuf->size - ringbuf->endIdx;
    cpSize = (right < size) ? right : size;

    ret = memcpy_s(ringbuf->fifo + ringbuf->endIdx, right, buf, cpSize);
    if (ret != OS_OK) {
        printf("write loop ring buffer failed, endIdx: %u, right:%u, cpSize=%u\n", ringbuf->endIdx, right, cpSize);
        return 0;
    }

    ringbuf->remain -= cpSize;
    ringbuf->endIdx += cpSize;

    if (ringbuf->endIdx == ringbuf->size) {
        ringbuf->endIdx = 0;
    }

    if (cpSize < size) {
        cpSize += OsRingbufWriteLinear(ringbuf, buf + cpSize, size - cpSize);
    }

    return cpSize;
}
