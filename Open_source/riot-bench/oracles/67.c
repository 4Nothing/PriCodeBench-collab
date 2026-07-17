unsigned crb_end_chunk(chunk_ringbuf_t *rb, bool keep)
{
    int idx;

    /* no chunk was started */
    if (rb->cur_start == NULL) {
        return 0;
    }

    if (keep) {
        idx = _get_free_chunk(rb);
    } else {
        idx = -1;
    }

    /* discard chunk */
    if (idx < 0) {
        if (rb->protect == rb->cur_start) {
            rb->protect = NULL;
        }
        rb->cur = rb->cur_start;
        rb->cur_start = NULL;
        return 0;
    }

    /* store complete chunk */
    rb->chunk_start[idx] = rb->cur_start;
    rb->chunk_len[idx] = _get_cur_len(rb);
    rb->cur_start = NULL;

    return rb->chunk_len[idx];
}