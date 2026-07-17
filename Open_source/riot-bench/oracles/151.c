int gnrc_ipv6_nib_pl_set(unsigned iface,
                         const ipv6_addr_t *pfx, unsigned pfx_len,
                         uint32_t valid_ltime, uint32_t pref_ltime)
{
    _nib_offl_entry_t *dst;
    ipv6_addr_t tmp = IPV6_ADDR_UNSPECIFIED;

    assert((pfx != NULL));
    if (pfx_len > IPV6_ADDR_BIT_LEN) {
        pfx_len = IPV6_ADDR_BIT_LEN;
    }
    ipv6_addr_init_prefix(&tmp, pfx, pfx_len);
    /* pfx_len == 0 implicitly checked, since this leaves tmp unspecified */
    if (ipv6_addr_is_unspecified(&tmp) || ipv6_addr_is_link_local(pfx) ||
        ipv6_addr_is_multicast(pfx) || (pref_ltime > valid_ltime)) {
        return -EINVAL;
    }
    _nib_acquire();
    dst = _nib_pl_add(iface, pfx, pfx_len, valid_ltime,
                      pref_ltime);
    if (dst == NULL) {
        _nib_release();
        return -ENOMEM;
    }
#ifdef MODULE_GNRC_NETIF
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

    if (netif == NULL) {
        _nib_release();
        return 0;
    }
    gnrc_netif_acquire(netif);
    /* prefixes within a 6Lo-ND-performing network are typically off-link, the
     * border router however should configure the prefix as on-link to only do
     * address resolution towards the LoWPAN and not the upstream interface
     * See https://github.com/RIOT-OS/RIOT/pull/10627 and follow-ups
     */
    if (!gnrc_netif_is_6ln(netif) || gnrc_netif_is_6lbr(netif)) {
        dst->flags |= _PFX_ON_LINK;
    }

    /* Auto-configuration only works if the prefix is more than a single address */
    if ((netif->ipv6.aac_mode & GNRC_NETIF_AAC_AUTO) && (pfx_len < 128)) {
        dst->flags |= _PFX_SLAAC;
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) && IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
    if (gnrc_netif_is_6lbr(netif)) {
        _nib_abr_entry_t *abr = NULL;

        while ((abr = _nib_abr_iter(abr))) {
            abr->version++;
            _nib_abr_add_pfx(abr, dst);
        }
    }
#endif
    gnrc_netif_release(netif);
#endif  /* MODULE_GNRC_NETIF */
    _nib_release();
#if defined(MODULE_GNRC_NETIF) && IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    /* update prefixes down-stream */
    _handle_snd_mc_ra(netif);
#endif
    return 0;
}
