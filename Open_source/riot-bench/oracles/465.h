static inline void *coap_get_token(const coap_pkt_t *pkt)
{
    return coap_hdr_data_ptr(coap_get_udp_hdr_const(pkt));
}
