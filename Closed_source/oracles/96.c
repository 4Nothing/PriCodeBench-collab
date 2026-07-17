void PRT_RingbufReset(Ringbuf *ringbuf)
{
    uintptr_t intSave;

    if ((ringbuf == NULL) || (ringbuf->status != RBUF_INITED)) {
        return;
    }

    intSave = PRT_SplIrqLock(&ringbuf->lock);
    ringbuf->startIdx = 0;
    ringbuf->endIdx = 0;
    ringbuf->remain = ringbuf->size;
    (void)memset_s(ringbuf->fifo, ringbuf->size, 0, ringbuf->size);
    PRT_SplIrqUnlock(&ringbuf->lock, intSave);

    return;
}
