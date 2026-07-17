void gnrc_ipv6_nib_pl_del(unsigned iface,
                          const ipv6_addr_t *pfx, unsigned pfx_len)
{
    _nib_offl_entry_t *dst = NULL;

    assert(pfx != NULL);
    _nib_acquire();
    while ((dst = _nib_offl_iter(dst)) != NULL) {
        assert(dst->next_hop != NULL);
        if ((pfx_len == dst->pfx_len) &&
            ((iface == 0) || (iface == _nib_onl_get_if(dst->next_hop))) &&
            (ipv6_addr_match_prefix(pfx, &dst->pfx) >= pfx_len)) {

            /* notify downstream nodes about the prefix removal */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
            gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

            if (netif && (netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV)) {
                _snd_rtr_advs_drop_pfx(netif, &ipv6_addr_all_nodes_link_local, dst);
            }
#endif
            /* remove the prefix & associated address*/
            _nib_offl_remove_prefix(dst);
            break;
        }
    }
    _nib_release();
}
