bool gnrc_ipv6_nib_ft_iter(const ipv6_addr_t *next_hop, unsigned iface,
                          void **state, gnrc_ipv6_nib_ft_t *fte)
{
    _nib_dr_entry_t *entry;
    assert((state != NULL) && (fte != NULL));

    if ((*state == NULL) || _nib_offl_is_entry(*state)) {
        _nib_offl_entry_t *offl = *state;

        while ((offl = _nib_offl_iter(offl))) {
            assert(offl->mode != 0);
            if (offl->next_hop == NULL) {
                /* 'holey' NIB / dangling reference.
                 * there is no next hop (not even an interface) */
                continue;
            }
            if (offl->mode == _PL && !(offl->flags & _PFX_ON_LINK)) {
                /* prefix list entry is off-link */
                continue;
            }
            if (iface && iface != _nib_onl_get_if(offl->next_hop)) {
                /* interface does not match */
                continue;
            }
            if (next_hop && !ipv6_addr_equal(&offl->next_hop->ipv6, next_hop)) {
                /* next hop does not match */
                continue;
            }
            _nib_ft_get(offl, fte);
            *state = offl;
            return true;
        }
        *state = NULL;
    }
    entry = *state;
    while ((entry = _nib_drl_iter(entry))) {
        if ((entry->next_hop != NULL) &&
            ((iface == 0) || (iface == _nib_onl_get_if(entry->next_hop))) &&
            ((next_hop == NULL) || ipv6_addr_equal(&entry->next_hop->ipv6,
                                                   next_hop))) {
            _nib_drl_ft_get(entry, fte);
            break;
        }
    }
    *state = entry;
    return (*state != NULL);
}
