static inline gnrc_pktsnip_t *gnrc_pkt_append(gnrc_pktsnip_t *pkt,
                                              gnrc_pktsnip_t *snip)
{
    /* find last snip in pkt */
    gnrc_pktsnip_t *last = gnrc_pkt_prev_snip(pkt, NULL);

    if (last != NULL) {
        last->next = snip;
    }
    else {
        /* last == NULL means snip */
        pkt = snip;
    }
    return pkt;
}
