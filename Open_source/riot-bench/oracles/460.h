static inline unsigned coap_get_code_decimal(const coap_pkt_t *pkt)
{
    return (coap_get_code_class(pkt) * 100) + coap_get_code_detail(pkt);
}
