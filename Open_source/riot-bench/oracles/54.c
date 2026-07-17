ssize_t coap_parse_udp(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    pkt->buf = buf;
    coap_udp_hdr_t *hdr = (coap_udp_hdr_t *)buf;

    uint8_t *pkt_pos = coap_hdr_data_ptr(hdr);
    uint8_t *pkt_end = buf + len;

    pkt->payload = NULL;
    pkt->payload_len = 0;
    memset(pkt->opt_crit, 0, sizeof(pkt->opt_crit));
    pkt->snips = NULL;

    if (len < sizeof(coap_udp_hdr_t)) {
        DEBUG("msg too short\n");
        return -EBADMSG;
    }
    else if ((coap_get_code_raw(pkt) == 0) && (len > sizeof(coap_udp_hdr_t))) {
        DEBUG("empty msg too long\n");
        return -EBADMSG;
    }

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        if ((coap_get_udp_hdr(pkt)->ver_t_tkl & 0xf) == 15) {
            DEBUG("nanocoap: token length is reserved value 15,"
                  "invalid for extended token length field.\n");
            return -EBADMSG;
        }
    } else if (coap_get_token_len(pkt) > COAP_TOKEN_LENGTH_MAX) {
        DEBUG("nanocoap: token length invalid\n");
        return -EBADMSG;
    }
    /* pkt_pos range is validated after options parsing loop below */
    pkt_pos += coap_get_token_len(pkt);

    coap_optpos_t *optpos = pkt->options;
    unsigned option_count = 0;
    unsigned option_nr = 0;

    /* parse options */
    while (pkt_pos < pkt_end) {
        uint8_t *option_start = pkt_pos;
        uint8_t option_byte = *pkt_pos++;
        if (option_byte == COAP_PAYLOAD_MARKER) {
            pkt->payload = pkt_pos;
            pkt->payload_len = buf + len - pkt_pos;
            DEBUG("payload len = %u\n", pkt->payload_len);
            break;
        }
        else {
            int option_delta = _decode_value(option_byte >> 4, &pkt_pos, pkt_end);
            if (option_delta < 0) {
                DEBUG("bad op delta\n");
                return -EBADMSG;
            }
            int option_len = _decode_value(option_byte & 0xf, &pkt_pos, pkt_end);
            if (option_len < 0) {
                DEBUG("bad op len\n");
                return -EBADMSG;
            }
            option_nr += option_delta;
            DEBUG("option count=%u nr=%u len=%i\n", option_count, option_nr, option_len);

            if (option_delta) {
                if (option_count >= CONFIG_NANOCOAP_NOPTS_MAX) {
                    DEBUG("nanocoap: max nr of options exceeded\n");
                    return -ENOMEM;
                }

                /* check if option is critical */
                if (option_nr & 1) {
                    bf_set(pkt->opt_crit, option_count);
                }
                optpos->opt_num = option_nr;
                optpos->offset = (uintptr_t)option_start - (uintptr_t)hdr;
                DEBUG("optpos option_nr=%u %u\n", (unsigned)option_nr, (unsigned)optpos->offset);
                optpos++;
                option_count++;
            }

            pkt_pos += option_len;
        }
    }

    if (pkt_pos > pkt_end) {
        DEBUG("nanocoap: bad packet length\n");
        return -EBADMSG;
    }

    pkt->options_len = option_count;
    if (!pkt->payload) {
        pkt->payload = pkt_pos;
    }

#ifdef MODULE_GCOAP
    if (coap_opt_get_uint(pkt, COAP_OPT_OBSERVE, &pkt->observe_value) != 0) {
        pkt->observe_value = UINT32_MAX;
    }
#endif

    DEBUG("coap pkt parsed. code=%u detail=%u payload_len=%u, nopts=%u, 0x%02x\n",
          coap_get_code_class(pkt),
          coap_get_code_detail(pkt),
          pkt->payload_len, option_count, hdr->code);

    return len;
}
