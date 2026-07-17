ssize_t coap_opt_remove(coap_pkt_t *pkt, uint16_t opt_num)
{
    assert(pkt);
    coap_optpos_t *optpos = pkt->options;
    coap_optpos_t *prev_opt = NULL;
    unsigned opt_count = pkt->options_len;
    uint8_t *start_old = NULL; /* First byte that is memmoved: content of next
                                * option, payload marker or end of message */
    uint8_t *start_new = NULL; /* memmove destination: content after rewritten
                                * next option header, or offset of removed option */

    while (opt_count--) {
        if ((start_old == NULL) && (optpos->opt_num == opt_num)) {
            unsigned new_hdr_len, old_hdr_len;
            int option_len;
            uint8_t *opt_start;
            uint16_t old_delta, new_delta;

            if (opt_count == 0) {
                /* this is the last option => use payload / end pointer as old start */
                start_old = (pkt->payload_len) ? pkt->payload - 1 : pkt->payload;
                start_new = pkt->buf + optpos->offset;
                break;
            }

            if (prev_opt == NULL) {
                /* this is the first option => new_delta is just opt_num of next option */
                new_delta = optpos[1].opt_num;
            } else {
                new_delta = optpos[1].opt_num - prev_opt->opt_num;
            }
            prev_opt = optpos;
            optpos++;
            opt_count--;
            /* select start of next option */
            opt_start = pkt->buf + optpos->offset;
            start_old = _parse_option(pkt, opt_start, &old_delta, &option_len);
            old_hdr_len = start_old - opt_start;

            /* select start of to be deleted option and set delta/length of next option */
            start_new = pkt->buf + prev_opt->offset;
            *start_new = 0;
            /* write new_delta value to option header: 4 upper bits of header (shift 4) +
             * 1 or 2 optional bytes depending on delta value) */
            new_hdr_len = _put_delta_optlen(start_new, 1, 4, new_delta);
            /* write option length to option header: 4 lower bits of header (shift 0) +
             * 1 or 2 optional bytes depending of the length of the option */
            new_hdr_len = _put_delta_optlen(start_new, new_hdr_len, 0, option_len);
            start_new += new_hdr_len;

            /* account for header length change in next option */
            optpos->offset -= (new_hdr_len - old_hdr_len);
        }
        /* start_old implies start_new */
        if (start_old != NULL) {
            assert(start_new);
            /* adapt options array for removed option */
            memcpy(prev_opt, optpos, sizeof(*prev_opt));
            prev_opt->offset -= (start_old - start_new);
        }
        prev_opt = optpos;
        optpos++;
    }
    if (start_old != NULL) {
        size_t move_size = (pkt->payload - start_old) + pkt->payload_len;

        pkt->options_len--;
        if (move_size > 0) {
            memmove(start_new, start_old, move_size);
        }
        pkt->payload -= (start_old - start_new);
    }
    return pkt->payload - pkt->buf + pkt->payload_len;
}
