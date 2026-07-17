static inline unsigned coap_get_type(const coap_pkt_t *pkt)
{
    return (coap_get_udp_hdr_const(pkt)->ver_t_tkl & 0x30) >> 4;
}
