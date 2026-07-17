ssize_t coap_opt_finish(coap_pkt_t *pkt, uint16_t flags)
{
    if (flags & COAP_OPT_FINISH_PAYLOAD) {
        if (!pkt->payload_len) {
            return -ENOSPC;
        }

        *pkt->payload++ = COAP_PAYLOAD_MARKER;
        pkt->payload_len--;
    }
    else {
        pkt->payload_len = 0;
    }

    return pkt->payload - pkt->buf;
}
