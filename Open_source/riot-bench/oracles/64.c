bool crb_add_bytes(chunk_ringbuf_t *rb, const void *data, size_t len)
{
    const uint8_t *in = data;
    for (size_t i = 0; i < len; ++i) {
        if (!crb_add_byte(rb ,in[i])) {
            return false;
        }
    }

    return true;
}