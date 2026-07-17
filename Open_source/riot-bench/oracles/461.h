static inline unsigned coap_get_code_raw(const coap_pkt_t *pkt)
{
    return coap_get_udp_hdr_const(pkt)->code;
}
