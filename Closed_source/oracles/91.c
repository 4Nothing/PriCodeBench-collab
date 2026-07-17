static U32 OsRingbufReadLoop(Ringbuf *ringbuf, char *buf, U32 size)
{
    U32 ret;
    U32 right;
    U32 cpSize;

    right = ringbuf->size - ringbuf->startIdx;
    cpSize = (right < size) ? right : size;

    ret = memcpy_s(buf, size, ringbuf->fifo + ringbuf->startIdx, cpSize);
    if (ret != OS_OK) {
        printf("read loop ring buffer failed, startIdx: %u, right:%u, cpSize=%u\n", ringbuf->startIdx, right, cpSize);
        return 0;
    }

    ringbuf->remain += cpSize;
    ringbuf->startIdx += cpSize;
    if (ringbuf->startIdx == ringbuf->size) {
        ringbuf->startIdx = 0;
    }

    if (cpSize < size) {
        cpSize += OsRingbufReadLinear(ringbuf, buf + cpSize, size - cpSize);
    }

    return cpSize;
}
