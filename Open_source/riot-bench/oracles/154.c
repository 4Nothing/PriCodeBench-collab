unsigned gnrc_netif_pktq_usage(void)
{
    unsigned res = 0;

    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE; i++) {
        if (_pool[i].pkt != NULL) {
            res++;
        }
    }
    return res;
}
