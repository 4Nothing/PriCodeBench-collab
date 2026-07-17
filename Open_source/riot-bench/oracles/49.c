ssize_t coap_opt_get_next(const coap_pkt_t *pkt, coap_optpos_t *opt,
                          uint8_t **value, bool init_opt)
{
    if (init_opt) {
        opt->opt_num = 0;
        opt->offset = coap_get_total_hdr_len(pkt);
    }
    uint8_t *start = pkt->buf + opt->offset;

    /* Find start of option value and value length. */
    uint16_t delta;
    int len;

    start = _parse_option(pkt, start, &delta, &len);
    if (!start) {
        return -ENOENT;
    }

    *value = start;
    opt->opt_num += delta;
    opt->offset = start + len - pkt->buf;
    return len;
}
