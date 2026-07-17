int gnrc_netif_pktq_push_back(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif != NULL);
    assert(pkt != NULL);

    gnrc_pktqueue_t *entry = _get_free_entry(pkt);

    if (entry == NULL) {
        return -1;
    }
    LL_PREPEND(netif->send_queue.queue, entry);
    return 0;
}
