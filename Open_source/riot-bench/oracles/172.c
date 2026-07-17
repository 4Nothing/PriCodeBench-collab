gnrc_pktsnip_t *gnrc_pktbuf_reverse_snips(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *reversed = NULL, *ptr = pkt;

    while (ptr != NULL) {
        gnrc_pktsnip_t *next;

        /* try to write-protect snip as its next-pointer is changed below */
        pkt = gnrc_pktbuf_start_write(ptr); /* use pkt as temporary variable */
        if (pkt == NULL) {
            gnrc_pktbuf_release(reversed);
            gnrc_pktbuf_release(ptr);
            return NULL;
        }
        /* switch around pointers */
        next = pkt->next;
        pkt->next = reversed;
        reversed = pkt;
        ptr = next;
    }
    return reversed;
}
