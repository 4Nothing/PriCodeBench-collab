U32 PRT_RingbufUsedSize(Ringbuf *ringbuf)
{
    U32 size;
    uintptr_t intSave;

    if ((ringbuf == NULL) || (ringbuf->status != RBUF_INITED)) {
        return 0;
    }

    intSave = PRT_SplIrqLock(&ringbuf->lock);
    size = ringbuf->size - ringbuf->remain;
    PRT_SplIrqUnlock(&ringbuf->lock, intSave);

    return size;
}
