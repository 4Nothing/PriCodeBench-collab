void gnrc_ipv6_nib_ft_del(const ipv6_addr_t *dst, unsigned dst_len)
{
    _nib_acquire();
    if ((dst == NULL) || (dst_len == 0) || ipv6_addr_is_unspecified(dst)) {
        _nib_dr_entry_t *entry = _nib_drl_get_dr();

        if (entry != NULL) {
            _nib_drl_remove(entry);
        }
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    else {
        _nib_offl_entry_t *entry = NULL;

        while ((entry = _nib_offl_iter(entry))) {
            if ((entry->pfx_len == dst_len) &&
                (ipv6_addr_match_prefix(&entry->pfx, dst) >= dst_len)) {
                _nib_ft_remove(entry);
                break;
            }
        }
    }
#endif
    _nib_release();
}
