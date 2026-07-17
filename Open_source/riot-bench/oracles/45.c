ssize_t coap_opt_add_proxy_uri(coap_pkt_t *pkt, const char *uri)
{
    assert(pkt);
    assert(uri);

    return _add_opt_pkt(pkt, COAP_OPT_PROXY_URI, (uint8_t *)uri, strlen(uri));
}
