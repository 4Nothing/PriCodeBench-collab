int gnrc_netif_pktq_put(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif != NULL);
    assert(pkt != NULL);

    gnrc_pktqueue_t *entry = _get_free_entry(pkt);

    if (entry == NULL) {
        return -1;
    }
    gnrc_pktqueue_add(&netif->send_queue.queue, entry);
    return 0;
}
