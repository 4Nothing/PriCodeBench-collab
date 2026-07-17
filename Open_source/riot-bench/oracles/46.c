ssize_t coap_opt_add_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t value)
{
    uint32_t tmp = value;
    unsigned tmp_len = _encode_uint(&tmp);
    return _add_opt_pkt(pkt, optnum, (uint8_t *)&tmp, tmp_len);
}
