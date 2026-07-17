static inline ssize_t coap_get_proxy_uri(coap_pkt_t *pkt, char **target)
{
    return coap_opt_get_opaque(pkt, COAP_OPT_PROXY_URI, (uint8_t **)target);
}
