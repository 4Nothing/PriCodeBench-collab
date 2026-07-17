static inline ssize_t coap_parse(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    return coap_parse_udp(pkt, buf, len);
}
