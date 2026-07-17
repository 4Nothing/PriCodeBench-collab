static inline unsigned coap_get_total_hdr_len(const coap_pkt_t *pkt)
{
    return sizeof(coap_udp_hdr_t) + coap_hdr_tkl_ext_len(pkt->hdr) +
           coap_get_token_len(pkt);
}
