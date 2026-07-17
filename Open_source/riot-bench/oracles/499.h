static inline size_t gnrc_pkt_count(const gnrc_pktsnip_t *pkt)
{
    size_t count = 0;

    while (pkt != NULL) {
        ++count;
        pkt = pkt->next;
    }

    return count;
}
