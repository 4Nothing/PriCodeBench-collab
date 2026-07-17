static inline gnrc_pktsnip_t *gnrc_pkt_prev_snip(gnrc_pktsnip_t *pkt,
                                                 gnrc_pktsnip_t *snip)
{
    while ((pkt != NULL) && (pkt->next != snip)) {
        pkt = pkt->next;
    }
    return pkt;
}
