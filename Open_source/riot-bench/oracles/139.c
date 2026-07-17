bool gnrc_ipv6_nib_abr_iter(void **state, gnrc_ipv6_nib_abr_t *entry)
{
    _nib_abr_entry_t *abr = *state;

    _nib_acquire();
    while ((abr = _nib_abr_iter(abr)) != NULL) {
        if (!ipv6_addr_is_unspecified(&abr->addr)) {
            memcpy(&entry->addr, &abr->addr, sizeof(entry->addr));
            entry->version = abr->version;
            entry->valid_until_ms = abr->valid_until_ms;
            break;
        }
    }
    _nib_release();
    *state = abr;
    return (*state != NULL);
}
