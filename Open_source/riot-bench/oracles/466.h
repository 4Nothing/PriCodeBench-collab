static inline unsigned coap_get_token_len(const coap_pkt_t *pkt)
{
    return coap_hdr_get_token_len((const coap_udp_hdr_t *)pkt->buf);
}
