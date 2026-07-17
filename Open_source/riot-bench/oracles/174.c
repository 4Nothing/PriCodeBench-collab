gnrc_pktsnip_t *gnrc_pktsnip_search_type(gnrc_pktsnip_t *pkt,
                                         gnrc_nettype_t type)
{
    while ((pkt != NULL) && (pkt->type != type)) {
        pkt = pkt->next;
    }
    return pkt;
}
