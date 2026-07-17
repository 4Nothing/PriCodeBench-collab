ssize_t coap_opt_get_opaque(coap_pkt_t *pkt, unsigned opt_num, uint8_t **value)
{
    uint8_t *start = coap_find_option(pkt, opt_num);
    if (!start) {
        return -ENOENT;
    }

    uint16_t delta;
    int len;

    *value = _parse_option(pkt, start, &delta, &len);
    if (!*value) {
        return -EINVAL;
    }

    return len;
}
