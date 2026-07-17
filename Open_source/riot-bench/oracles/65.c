bool crb_add_chunk(chunk_ringbuf_t *rb, const void *data, size_t len)
{
    if (!crb_start_chunk(rb)) {
        return false;
    }

    bool keep = crb_add_bytes(rb ,data, len);

    return crb_end_chunk(rb ,keep);
}