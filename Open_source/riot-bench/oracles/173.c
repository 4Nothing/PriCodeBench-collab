gnrc_pktsnip_t *gnrc_pktbuf_start_write(gnrc_pktsnip_t *pkt)
{
    mutex_lock(&gnrc_pktbuf_mutex);
    if (pkt == NULL) {
        mutex_unlock(&gnrc_pktbuf_mutex);
        return NULL;
    }

    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE &&
        pkt->users == GNRC_PKTBUF_CANARY) {
        puts("gnrc_pktbuf: use after free detected\n");
        DEBUG_BREAKPOINT(3);
    }

    if (pkt->users > 1) {
        gnrc_pktsnip_t *new;
        new = _create_snip(pkt->next, pkt->data, pkt->size, pkt->type);
        if (new != NULL) {
            pkt->users--;
        }
        mutex_unlock(&gnrc_pktbuf_mutex);
        return new;
    }
    mutex_unlock(&gnrc_pktbuf_mutex);
    return pkt;
}