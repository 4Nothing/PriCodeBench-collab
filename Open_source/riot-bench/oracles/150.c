bool gnrc_ipv6_nib_pl_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_pl_t *entry)
{
    _nib_offl_entry_t *dst = *state;

    _nib_acquire();
    while ((dst = _nib_offl_iter(dst)) != NULL) {
        const _nib_onl_entry_t *node = dst->next_hop;
        if ((node != NULL) && (dst->mode & _PL) &&
            ((iface == 0) || (_nib_onl_get_if(node) == iface))) {
            entry->pfx_len = dst->pfx_len;
            ipv6_addr_set_unspecified(&entry->pfx);
            ipv6_addr_init_prefix(&entry->pfx, &dst->pfx, dst->pfx_len);
            entry->iface = _nib_onl_get_if(node);
            entry->valid_until = dst->valid_until;
            entry->pref_until = dst->pref_until;
            break;
        }
    }
    _nib_release();
    *state = dst;
    return (*state != NULL);
}
