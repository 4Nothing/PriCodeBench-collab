bool gnrc_ipv6_nib_nc_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_nc_t *entry)
{
    _nib_onl_entry_t *node = *state;

    _nib_acquire();
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((node->mode & _NC) &&
            ((iface == 0) || (_nib_onl_get_if(node) == iface))) {
            _nib_nc_get(node, entry);
            break;
        }
    }
    *state = node;
    _nib_release();
    return (*state != NULL);
}
