bool crb_consume_chunk(chunk_ringbuf_t *rb, void *dst, size_t len)
{
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    if (len > rb->chunk_len[idx]) {
        len = rb->chunk_len[idx];
    }

    if (dst) {
        if (rb->chunk_start[idx] + len <= rb->buffer_end) {
            /* chunk is continuous */
            memcpy(dst, rb->chunk_start[idx], len);
        } else {
            /* chunk wraps around */
            uint8_t *dst8 = dst;
            size_t len_0 = 1 + rb->buffer_end - rb->chunk_start[idx];
            memcpy(dst8, rb->chunk_start[idx], len_0);
            memcpy(dst8 + len_0, rb->buffer, len - len_0);
        }
    }

    unsigned state = irq_disable();

    rb->chunk_start[idx] = NULL;

    /* advance protect marker */
    idx = _get_complete_chunk(rb);
    if (idx < 0) {
        rb->protect = rb->cur_start;
    } else {
        rb->protect = rb->chunk_start[idx];
    }

    /* advance first used slot nr */
    rb->chunk_cur = (rb->chunk_cur + 1) % CONFIG_CHUNK_NUM_MAX;

    irq_restore(state);

    return true;
}