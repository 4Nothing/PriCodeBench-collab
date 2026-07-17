static inline ssize_t coap_get_location_path(coap_pkt_t *pkt,
                                             uint8_t *target, size_t max_len)
{
    return coap_opt_get_string(pkt, COAP_OPT_LOCATION_PATH,
                               target, max_len, '/');
}
