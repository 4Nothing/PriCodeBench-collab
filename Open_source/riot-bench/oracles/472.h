static inline ssize_t coap_opt_add_format(coap_pkt_t *pkt, uint16_t format)
{
    return coap_opt_add_uint(pkt, COAP_OPT_CONTENT_FORMAT, format);
}
