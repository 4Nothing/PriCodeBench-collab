bool crb_get_chunk_size(chunk_ringbuf_t *rb, size_t *len)
{
    int idx = _get_complete_chunk(rb);
    if (idx < 0) {
        return false;
    }

    *len = rb->chunk_len[idx];
    return true;
}