static inline gnrc_pktsnip_t *gnrc_netif_pktq_get(gnrc_netif_t *netif)
{
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    assert(netif != NULL);

    gnrc_pktsnip_t *pkt = NULL;
    gnrc_pktqueue_t *entry = gnrc_pktqueue_remove_head(
        &netif->send_queue.queue
    );

    if (entry != NULL) {
        pkt = entry->pkt;
        entry->pkt = NULL;
    }
    return pkt;
#else   /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
    (void)netif;
    return NULL;
#endif  /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
}
