static inline gnrc_pktsnip_t *gnrc_pkt_prepend(gnrc_pktsnip_t *pkt,
                                               gnrc_pktsnip_t *snip)
{
    snip->next = pkt;
    return snip;
}
