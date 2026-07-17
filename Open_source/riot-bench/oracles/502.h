static inline size_t gnrc_pkt_len_upto(const gnrc_pktsnip_t *pkt, gnrc_nettype_t type)
{
    size_t len = 0;

    while (pkt != NULL) {
        len += pkt->size;

        if (pkt->type == type) {
            break;
        }

        pkt = pkt->next;
    }

    return len;
}
