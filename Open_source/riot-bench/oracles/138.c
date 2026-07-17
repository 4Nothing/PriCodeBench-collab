void gnrc_ipv6_nib_abr_del(const ipv6_addr_t *addr)
{
    _nib_acquire();
    _nib_abr_remove(addr);
    _nib_release();
}
