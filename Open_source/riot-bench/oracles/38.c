ssize_t coap_build_reply_header(coap_pkt_t *pkt, unsigned code,
                                void *buf, size_t len,
                                uint16_t ct,
                                void **payload, size_t *payload_len_max)
{
    uint8_t *bufpos = buf;
    uint32_t no_response;
    unsigned tkl = coap_get_token_len(pkt);
    size_t hdr_len = sizeof(coap_udp_hdr_t) + tkl;
    uint8_t type = coap_get_type(pkt) == COAP_TYPE_CON
                 ? COAP_TYPE_ACK
                 : COAP_TYPE_NON;

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        /* Worst case: 2 byte extended token length field.
         * See https://www.rfc-editor.org/rfc/rfc8974#name-extended-token-length-tkl-f
         */
        hdr_len += 2;
    }

    if (hdr_len > len) {
        return -ENOBUFS;
    }

    bufpos += coap_build_udp_hdr(buf, len, type, coap_get_token(pkt), tkl,
                                 code, coap_get_id(pkt));

    if (coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response) == 0) {
        const uint8_t no_response_index = (code >> 5) - 1;
        /* If the handler code misbehaved here, we'd face UB otherwise */
        assume(no_response_index < 7);

        const uint8_t mask = 1 << no_response_index;
        if (no_response & mask) {
            if (payload) {
                *payload = NULL;
                *payload_len_max = 0;
                payload = NULL;
            }

            return -ECANCELED;
        }
    }

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        /* we need to update the header length with the actual one, as we may
         * have used less bytes for the extended token length fields as our
         * worst case assumption */
        hdr_len  = bufpos - (uint8_t *)buf;
    }

    if (payload) {
        if (ct != COAP_FORMAT_NONE) {
            bufpos += coap_put_option_ct(bufpos, 0, ct);
        }
        *bufpos++ = COAP_PAYLOAD_MARKER;
        *payload = bufpos;
        hdr_len  = bufpos - (uint8_t *)buf;
        *payload_len_max = len - hdr_len;
    }

    /* with the nanoCoAP API we can't detect the overflow before it happens */
    assert(hdr_len <= len);

    return hdr_len;
}
