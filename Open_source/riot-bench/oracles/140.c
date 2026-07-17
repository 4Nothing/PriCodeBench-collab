int gnrc_ipv6_nib_ft_add(const ipv6_addr_t *dst, unsigned dst_len,
                         const ipv6_addr_t *next_hop, unsigned iface,
                         uint32_t ltime)
{
    int res = 0;
    bool is_default_route = ((dst == NULL) || (dst_len == 0) ||
                             ipv6_addr_is_unspecified(dst));

    uint32_t ltime_ms;
    /* The valid lifetime is given in seconds, but our timers work in
     * milliseconds, so we have to scale down to the smallest possible
     * value (UINT32_MAX ms). This is however alright since we ask for
     * a new router advertisement before this timeout expires */
    if (ltime > UINT32_MAX / MS_PER_SEC) {
        ltime_ms = UINT32_MAX;
    }
    else {
        ltime_ms = ltime * MS_PER_SEC;
    }
    if ((iface == 0) || ((is_default_route) && (next_hop == NULL))) {
        return -EINVAL;
    }
    _nib_acquire();
    if (is_default_route) {
        _nib_dr_entry_t *ptr;

        ptr = _nib_drl_add(next_hop, iface);
        if (ptr == NULL) {
            res = -ENOMEM;
        }
        else {
            _prime_def_router = ptr;
            if (ltime_ms > 0) {
                _evtimer_add(ptr, GNRC_IPV6_NIB_RTR_TIMEOUT,
                             &ptr->rtr_timeout, ltime_ms);
            }
        }
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    else {
        _nib_offl_entry_t *ptr;

        dst_len = (dst_len > 128) ? 128 : dst_len;
        ptr = _nib_ft_add(next_hop, iface, dst, dst_len);
        if (ptr == NULL) {
            res = -ENOMEM;
        }
        else if (ltime_ms > 0) {
            _evtimer_add(ptr, GNRC_IPV6_NIB_ROUTE_TIMEOUT,
                         &ptr->route_timeout, ltime_ms);
        }
    }
#else /* CONFIG_GNRC_IPV6_NIB_ROUTER */
    else {
        res = -ENOTSUP;
    }
#endif
    _nib_release();
    return res;
}
