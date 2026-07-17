static inline ssize_t coap_opt_add_uri_query(coap_pkt_t *pkt, const char *key,
                                             const char *val)
{
    return coap_opt_add_uri_query2(pkt, key, strlen(key), val, val ? strlen(val) : 0);
}
