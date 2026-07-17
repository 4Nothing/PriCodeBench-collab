static inline bool crb_start_chunk(chunk_ringbuf_t *rb)
{
    /* discard stale chunk */
    if (rb->cur_start) {
        crb_end_chunk(rb, false);
    }

    /* pointing to the start of the first chunk */
    if (rb->cur == rb->protect) {
        return false;
    }

    rb->cur_start = rb->cur;

    if (rb->protect == NULL) {
        rb->protect = rb->cur_start;
    }

    return true;
}