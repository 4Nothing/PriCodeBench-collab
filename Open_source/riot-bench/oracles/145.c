void gnrc_ipv6_nib_nc_del(const ipv6_addr_t *ipv6, unsigned iface)
{
    _nib_onl_entry_t *node = NULL;

    _nib_acquire();
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((_nib_onl_get_if(node) == iface) &&
            ipv6_addr_equal(ipv6, &node->ipv6)) {
            _nib_nc_remove(node);
            break;
        }
    }
    _nib_release();
}
