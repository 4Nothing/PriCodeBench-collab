int gnrc_ipv6_nib_abr_add(const ipv6_addr_t *addr)
{
    _nib_abr_entry_t *abr;
    _nib_offl_entry_t *offl = NULL;
    gnrc_netif_t *netif = gnrc_netif_get_by_ipv6_addr(addr);

    assert(netif != NULL);
    _nib_acquire();
    if ((abr = _nib_abr_add(addr)) == NULL) {
        _nib_release();
        return -ENOMEM;
    }
    abr->valid_until_ms = evtimer_now_msec() + (
        SIXLOWPAN_ND_OPT_ABR_LTIME_DEFAULT * MS_PER_SEC * SEC_PER_MIN
    );
    /* Associate all existing prefixes in the prefix list of the border router's
     * downstream interface to the authoritative border router so they are
     * advertised in a Router Advertisement with the Authoritative Border Router
     * Option (ABRO) in a respective Prefix Information Option (PIO)
     * (see https://tools.ietf.org/html/rfc6775#section-8.1.1). */
    while ((offl = _nib_offl_iter(offl))) {
        if ((offl->mode & _PL) &&
            (_nib_onl_get_if(offl->next_hop) == (unsigned)netif->pid)) {
            _nib_abr_add_pfx(abr, offl);
        }
    }
#ifdef MODULE_GNRC_SIXLOWPAN_CTX    /* included optionally for NIB testing */
    for (uint8_t id = 0; id < GNRC_SIXLOWPAN_CTX_SIZE; id++) {
        if (gnrc_sixlowpan_ctx_lookup_id(id) != NULL) {
            bf_set(abr->ctxs, id);
        }
    }
#endif  /* MODULE_GNRC_SIXLOWPAN_CTX */
    _nib_release();
    return 0;
}
