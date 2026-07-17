int gnrc_pktbuf_merge(gnrc_pktsnip_t *pkt)
{
    size_t offset = pkt->size;
    size_t size = gnrc_pkt_len(pkt);
    int res = 0;

    if (pkt->size == size) {
        return res;
    }

    /* Re-allocate data */
    res = gnrc_pktbuf_realloc_data(pkt, size);
    if (res != 0) {
        return res;
    }

    /* Copy data to new buffer */
    for (gnrc_pktsnip_t *ptr = pkt->next; ptr != NULL; ptr = ptr->next) {
        memcpy(((uint8_t *)pkt->data) + offset, ptr->data, ptr->size);
        offset += ptr->size;
    }

    /* Release old pktsnips and data*/
    gnrc_pktbuf_release(pkt->next);
    pkt->next = NULL;
    return res;
}
