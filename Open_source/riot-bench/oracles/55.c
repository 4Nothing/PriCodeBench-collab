void coap_pkt_init(coap_pkt_t *pkt, uint8_t *buf, size_t len, size_t header_len)
{
    memset(pkt, 0, sizeof(coap_pkt_t));
    pkt->buf = buf;
    pkt->payload = buf + header_len;
    pkt->payload_len = len - header_len;
}
