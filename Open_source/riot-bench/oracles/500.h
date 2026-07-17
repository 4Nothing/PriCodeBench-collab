static inline gnrc_pktsnip_t *gnrc_pkt_delete(gnrc_pktsnip_t *pkt,
                                              gnrc_pktsnip_t *snip)
{
    /* Removing head is a no-op. The new head is the next in the list. */
    if (pkt == snip) {
        return pkt->next;
    }

    /* Removing nothing is a no-op, the new head is the old one */
    if (snip == NULL) {
        return pkt;
    }

    /* Iterate over the list and remove the given snip from it, if found.
     * The new head is the old head. */
    for (gnrc_pktsnip_t *i = pkt; i != NULL; i = i->next) {
        if (i->next == snip) {
            i->next = snip->next;
            return pkt;
        }
    }

    return pkt;
}
