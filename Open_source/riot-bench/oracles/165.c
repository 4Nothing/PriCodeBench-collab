void gnrc_pktbuf_init(void)
{
    mutex_lock(&gnrc_pktbuf_mutex);
    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE) {
        memset(_static_buf, GNRC_PKTBUF_CANARY, sizeof(_static_buf));
    }
    /* Silence false -Wcast-align: _static_buf has qualifier
     * `alignas(_unused_t)`, so it is guaranteed to be safe */
    _first_unused = (_unused_t *)(uintptr_t)_static_buf;
    _first_unused->next = NULL;
    _first_unused->size = sizeof(_static_buf);
    mutex_unlock(&gnrc_pktbuf_mutex);
}