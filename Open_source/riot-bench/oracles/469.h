static inline ssize_t coap_get_uri_path(coap_pkt_t *pkt, uint8_t *target)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_PATH, target,
                               CONFIG_NANOCOAP_URI_MAX, '/');
}
