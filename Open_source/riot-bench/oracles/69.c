void crb_init(chunk_ringbuf_t *rb, void *buffer, size_t len)
{
    memset(rb ,0, sizeof(*rb));
    rb->buffer = buffer;
    rb->buffer_end = &rb->buffer[len - 1];
    rb->cur = rb->buffer;
}