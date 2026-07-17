U32 PRT_RingbufInit(Ringbuf *ringbuf, char *fifo, U32 size)
{
    if ((ringbuf == NULL) || (fifo == NULL) || (ringbuf->status == RBUF_INITED) || (size == 0)) {
        return OS_ERROR;
    }

    (void)memset_s(ringbuf, sizeof(Ringbuf), 0, sizeof(Ringbuf));
    OsSpinLockInitInner(&ringbuf->lock.rawLock);
    ringbuf->size = size;
    ringbuf->remain = size;
    ringbuf->fifo = fifo;
    ringbuf->status = RBUF_INITED;

    return OS_OK;
}
