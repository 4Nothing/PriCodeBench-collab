static inline ssize_t coap_get_uri_query_string(coap_pkt_t *pkt, char *target,
                                                size_t max_len)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_QUERY,
                               (uint8_t *)target, max_len, '&');
}
