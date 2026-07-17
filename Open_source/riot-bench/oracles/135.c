ipv6_hdr_t *gnrc_ipv6_get_header(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *tmp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    if (tmp == NULL) {
        return NULL;
    }

    assert(tmp->data != NULL);
    assert(tmp->size >= sizeof(ipv6_hdr_t));
    assert(ipv6_hdr_is(tmp->data));

    return ((ipv6_hdr_t*) tmp->data);
}