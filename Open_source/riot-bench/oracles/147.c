void gnrc_ipv6_nib_nc_mark_reachable(const ipv6_addr_t *ipv6)
{
    _nib_onl_entry_t *node = NULL;

    _nib_acquire();
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((node->mode & _NC) && ipv6_addr_equal(ipv6, &node->ipv6)) {
            /* only set reachable if not unmanaged */
            if ((node->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK)) {
                _nib_nc_set_reachable(node);
            }
            break;
        }
    }
    _nib_release();
}
