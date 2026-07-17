unsigned coap_get_content_type(coap_pkt_t *pkt)
{
    return _get_content_format(pkt, COAP_OPT_CONTENT_FORMAT);
}
