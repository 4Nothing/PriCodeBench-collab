static inline unsigned coap_get_id(const coap_pkt_t *pkt)
{
    return ntohs(coap_get_udp_hdr_const(pkt)->id);
}
