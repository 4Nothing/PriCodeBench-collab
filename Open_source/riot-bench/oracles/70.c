bool crb_peek_bytes(chunk_ringbuf_t *rb, void *dst, size_t offset, size_t len)
{
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    if (offset + len > rb->chunk_len[idx]) {
        return false;
    }

    const uint8_t *start = rb->chunk_start[idx];
    start += offset;

    if (start > rb->buffer_end) {
        start = ((uint8_t *)rb->buffer) + (start - rb->buffer_end + 1);
        memcpy(dst, start, len);
    } else if (start + len <= rb->buffer_end) {
        memcpy(dst, start, len);
    } else {
        size_t len_0 = 1 + rb->buffer_end - start;
        memcpy(dst, start, len_0);
        memcpy((uint8_t *)dst + len_0, rb->buffer, len - len_0);
    }

    return true;
}