int gnrc_pktbuf_realloc_data(gnrc_pktsnip_t *pkt, size_t size)
{
    size_t aligned_size = _align(size);

    mutex_lock(&gnrc_pktbuf_mutex);
    assert(pkt != NULL);
    assert(((pkt->size == 0) && (pkt->data == NULL)) ||
           ((pkt->size > 0) && (pkt->data != NULL) && gnrc_pktbuf_contains(pkt->data)));
    /* new size and old size are equal */
    if (size == pkt->size) {
        /* nothing to do */
        mutex_unlock(&gnrc_pktbuf_mutex);
        return 0;
    }
    /* new size is 0 and data pointer isn't already NULL */
    if ((size == 0) && (pkt->data != NULL)) {
        /* set data pointer to NULL */
        gnrc_pktbuf_free_internal(pkt->data, pkt->size);
        pkt->data = NULL;
    }
    /* if new size is bigger than old size */
    else if (size > pkt->size) {    /* new size does not fit */
        void *new_data = _pktbuf_alloc(size);
        if (new_data == NULL) {
            DEBUG("pktbuf: error allocating new data section\n");
            mutex_unlock(&gnrc_pktbuf_mutex);
            return ENOMEM;
        }
        if (pkt->data != NULL) {            /* if old data exist */
            memcpy(new_data, pkt->data, (pkt->size < size) ? pkt->size : size);
        }
        gnrc_pktbuf_free_internal(pkt->data, pkt->size);
        pkt->data = new_data;
    }
    else if (_align(pkt->size) > aligned_size) {
        gnrc_pktbuf_free_internal(((uint8_t *)pkt->data) + aligned_size,
                     pkt->size - aligned_size);
    }
    pkt->size = size;
    mutex_unlock(&gnrc_pktbuf_mutex);
    return 0;
}