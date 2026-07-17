static inline bool crb_add_byte(chunk_ringbuf_t *rb, uint8_t b)
{
    /* if this is the first chunk, protect will be at start */
    if (rb->cur == rb->protect &&
        rb->cur != rb->cur_start) {
        return false;
    }

    *rb->cur = b;

    /* handle wrap around */
    if (rb->cur == rb->buffer_end) {
        rb->cur = rb->buffer;
    } else {
        ++rb->cur;
    }

    return true;
}