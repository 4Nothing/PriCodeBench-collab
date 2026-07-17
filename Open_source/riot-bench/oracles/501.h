static inline size_t gnrc_pkt_len(const gnrc_pktsnip_t *pkt)
{
    size_t len = 0;

    while (pkt != NULL) {
        len += pkt->size;
        pkt = pkt->next;
    }

    return len;
}
