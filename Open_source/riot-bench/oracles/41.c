unsigned coap_get_accept(coap_pkt_t *pkt)
{
    return _get_content_format(pkt, COAP_OPT_ACCEPT);
}
