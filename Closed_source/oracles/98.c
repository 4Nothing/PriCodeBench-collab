U32 PRT_RingbufWrite(Ringbuf *ringbuf, const char *buf, U32 size)
{
    U32 cpSize;
    uintptr_t intSave;

    if ((ringbuf == NULL) || (buf == NULL) || (size == 0) || (ringbuf->fifo == NULL) ||
        (ringbuf->status != RBUF_INITED) || (ringbuf->remain == 0)) {
        return 0;
    }

    intSave = PRT_SplIrqLock(&ringbuf->lock);
    if (ringbuf->startIdx <= ringbuf->endIdx) {
        cpSize = OsRingbufWriteLoop(ringbuf, buf, size);
    } else {
        cpSize = OsRingbufWriteLinear(ringbuf, buf, size);
    }
    PRT_SplIrqUnlock(&ringbuf->lock, intSave);

    return cpSize;
}
