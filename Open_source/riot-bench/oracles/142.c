int gnrc_ipv6_nib_ft_get(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt,
                         gnrc_ipv6_nib_ft_t *fte)
{
    int res;

    assert((dst != NULL) && (fte != NULL));
    _nib_acquire();
    res = _nib_get_route(dst, pkt, fte);
    _nib_release();
    return res;
}
