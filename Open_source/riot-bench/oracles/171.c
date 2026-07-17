gnrc_pktsnip_t *gnrc_pktbuf_remove_snip(gnrc_pktsnip_t *pkt,
                                        gnrc_pktsnip_t *snip)
{
    pkt = gnrc_pkt_delete(pkt, snip);
    snip->next = NULL;
    gnrc_pktbuf_release(snip);

    return pkt;
}
