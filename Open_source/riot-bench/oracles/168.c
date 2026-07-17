gnrc_pktsnip_t *gnrc_pktbuf_mark(gnrc_pktsnip_t *pkt, size_t size, gnrc_nettype_t type)
{
    gnrc_pktsnip_t *marked_snip;
    /* size required for chunk */
    size_t required_new_size = _align(size);
    void *new_data_marked;

    mutex_lock(&gnrc_pktbuf_mutex);
    if ((size == 0) || (pkt == NULL) || (size > pkt->size) || (pkt->data == NULL)) {
        DEBUG("pktbuf: size == 0 (was %" PRIuSIZE ") or pkt == NULL (was %p) or "
              "size > pkt->size (was %" PRIuSIZE ") or pkt->data == NULL (was %p)\n",
              size, (void *)pkt, (pkt ? pkt->size : 0),
              (pkt ? pkt->data : NULL));
        mutex_unlock(&gnrc_pktbuf_mutex);
        return NULL;
    }
    /* create new snip descriptor for marked data */
    marked_snip = _pktbuf_alloc(sizeof(gnrc_pktsnip_t));
    if (marked_snip == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        mutex_unlock(&gnrc_pktbuf_mutex);
        return NULL;
    }
    /* marked data would not fit _unused_t marker => move data around to allow
     * for proper free */
    if ((pkt->size != size) && (size < required_new_size)) {
        void *new_data_rest;
        new_data_marked = _pktbuf_alloc(size);
        if (new_data_marked == NULL) {
            DEBUG("pktbuf: could not reallocate marked section.\n");
            gnrc_pktbuf_free_internal(marked_snip, sizeof(gnrc_pktsnip_t));
            mutex_unlock(&gnrc_pktbuf_mutex);
            return NULL;
        }
        new_data_rest = _pktbuf_alloc(pkt->size - size);
        if (new_data_rest == NULL) {
            DEBUG("pktbuf: could not reallocate remaining section.\n");
            gnrc_pktbuf_free_internal(marked_snip, sizeof(gnrc_pktsnip_t));
            gnrc_pktbuf_free_internal(new_data_marked, size);
            mutex_unlock(&gnrc_pktbuf_mutex);
            return NULL;
        }
        memcpy(new_data_marked, pkt->data, size);
        memcpy(new_data_rest, ((uint8_t *)pkt->data) + size, pkt->size - size);
        gnrc_pktbuf_free_internal(pkt->data, pkt->size);
        marked_snip->data = new_data_marked;
        pkt->data = new_data_rest;
    }
    else {
        new_data_marked = pkt->data;
        /* if (pkt->size - size) != 0 take remainder of data, otherwise set NULL */
        pkt->data = (pkt->size != size) ? (((uint8_t *)pkt->data) + size) :
                                          NULL;
    }
    pkt->size -= size;
    _set_pktsnip(marked_snip, pkt->next, new_data_marked, size, type);
    pkt->next = marked_snip;
    mutex_unlock(&gnrc_pktbuf_mutex);
    return marked_snip;
}