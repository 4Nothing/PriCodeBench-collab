int coap_opt_get_uint(coap_pkt_t *pkt, uint16_t opt_num, uint32_t *target)
{
    assert(target);

    uint8_t *opt_pos = coap_find_option(pkt, opt_num);
    if (opt_pos) {
        uint16_t delta;
        int option_len = 0;
        uint8_t *pkt_pos = _parse_option(pkt, opt_pos, &delta, &option_len);
        if (option_len >= 0) {
            if (option_len > 4) {
                DEBUG("nanocoap: uint option with len > 4 (unsupported).\n");
                return -ENOSPC;
            }
            *target = _decode_uint(pkt_pos, option_len);
            return 0;
        }
        else {
            DEBUG("nanocoap: discarding packet with invalid option length.\n");
            return -EBADMSG;
        }
    }
    return -ENOENT;
}
