static inline unsigned coap_get_code_class(const coap_pkt_t *pkt)
{
    return coap_get_code_raw(pkt) >> 5;
}
