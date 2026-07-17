ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned payload_len)
{
    unsigned tkl = coap_get_token_len(pkt);
    unsigned type = COAP_TYPE_NON;

    if (!code) {
        /* if code is COAP_CODE_EMPTY (zero), assume Reset (RST) type.
         * RST message have no token */
        type = COAP_TYPE_RST;
        tkl = 0;
    }
    else if (coap_get_type(pkt) == COAP_TYPE_CON) {
        type = COAP_TYPE_ACK;
    }
    unsigned len = sizeof(coap_udp_hdr_t) + tkl;

    if ((len + payload_len) > rlen) {
        return -ENOSPC;
    }

    uint32_t no_response;
    if (coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response) == 0) {

        const uint8_t no_response_index = (code >> 5) - 1;
        /* If the handler code misbehaved here, we'd face UB otherwise */
        assert(no_response_index < 7);

        const uint8_t mask = 1 << no_response_index;

        /* option contains bitmap of disinterest */
        if (no_response & mask) {
            return -ECANCELED;
        }
    }

    coap_build_udp_hdr(rbuf, rlen, type, coap_get_token(pkt), tkl, code, coap_get_id(pkt));
    len += payload_len;

    /* HACK: many CoAP handlers assume that the pkt buffer is also used for the response */
    pkt->hdr = (void *)rbuf;

    return len;
}
