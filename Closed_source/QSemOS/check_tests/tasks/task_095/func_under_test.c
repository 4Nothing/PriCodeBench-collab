U32 PRT_RingbufRead(Ringbuf *ringbuf, char *buf, U32 size)
{
    U32 cpSize;
    uintptr_t intSave;

    if ((ringbuf == NULL) || (buf == NULL) || (size == 0) || (ringbuf->fifo == NULL) ||
        (ringbuf->status != RBUF_INITED) || (ringbuf->remain == ringbuf->size)) {
        return 0;
    }

    intSave = PRT_SplIrqLock(&ringbuf->lock);
    if (ringbuf->startIdx >= ringbuf->endIdx) {
        cpSize = OsRingbufReadLoop(ringbuf, buf, size);
    } else {
        cpSize = OsRingbufReadLinear(ringbuf, buf, size);
    }
    PRT_SplIrqUnlock(&ringbuf->lock, intSave);

    return cpSize;
}
