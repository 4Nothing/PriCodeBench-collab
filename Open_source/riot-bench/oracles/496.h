static inline bool gnrc_netif_pktq_empty(gnrc_netif_t *netif)
{
#if IS_USED(MODULE_GNRC_NETIF_PKTQ)
    assert(netif != NULL);

    return (netif->send_queue.queue == NULL);
#else   /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
    (void)netif;
    return false;
#endif  /* IS_USED(MODULE_GNRC_NETIF_PKTQ) */
}
