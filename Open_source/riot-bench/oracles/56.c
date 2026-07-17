ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          uint16_t ct,
                          const void *payload, size_t payload_len)
{
    void *payload_start;
    size_t payload_len_max;

    ssize_t header_len = coap_build_reply_header(pkt, code, buf, len, ct,
                                                payload ? &payload_start : NULL,
                                                &payload_len_max);
    if (payload == NULL || header_len <= 0) {
        return header_len;
    }

    if (payload_len > payload_len_max) {
        return -ENOBUFS;
    }

    memcpy(payload_start, payload, payload_len);

    return header_len + payload_len;
}
