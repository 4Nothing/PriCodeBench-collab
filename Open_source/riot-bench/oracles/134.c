int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code)
{
    int header_len = coap_build_reply(pdu, code, buf, len, 0);

    /* request contained no-response option or not enough space for response */
    if (header_len <= 0) {
        return -1;
    }

    pdu->options_len = 0;
    pdu->payload     = buf + header_len;
    pdu->payload_len = len - header_len;

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        _add_generated_observe_option(pdu);
    }

    return 0;
}
