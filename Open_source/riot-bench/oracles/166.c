bool gnrc_pktbuf_is_empty(void)
{
    return ((uintptr_t)_first_unused == (uintptr_t)_static_buf) &&
           (_first_unused->size == sizeof(_static_buf));
}