ssize_t coap_build_udp_hdr(void *buf, size_t buf_len, uint8_t type,
                           const void *token, size_t token_len,
                           uint8_t code, uint16_t id)
{
    assert(!(type & ~0x3));

    uint16_t tkl_ext;
    uint8_t tkl_ext_len, tkl;

    if (token_len > 268 && IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        tkl_ext_len = 2;
        tkl_ext = htons(token_len - 269); /* 269 = 255 + 14 */
        tkl = 14;
    }
    else if (token_len > 12 && IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        tkl_ext_len = 1;
        tkl_ext = token_len - 13;
        tkl = 13;
    }
    else {
        tkl = token_len;
        tkl_ext_len = 0;
    }

    size_t hdr_len = sizeof(coap_udp_hdr_t) + token_len + tkl_ext_len;
    if (buf_len < hdr_len) {
        return -EOVERFLOW;
    }

    memset(buf, 0, sizeof(coap_udp_hdr_t));
    coap_udp_hdr_t *hdr = buf;
    hdr->ver_t_tkl = (COAP_V1 << 6) | (type << 4) | tkl;
    hdr->code = code;
    hdr->id = htons(id);
    buf += sizeof(coap_udp_hdr_t);

    if (tkl_ext_len) {
        memcpy(buf, &tkl_ext, tkl_ext_len);
        buf += tkl_ext_len;
    }

    /* Some users build a response packet in the same buffer that contained
     * the request. In this case, the argument token already points inside
     * the target, or more specifically, it is already at the correct place.
     * Having `src` and `dest` in `memcpy(dest, src, len)` overlap is
     * undefined behavior, so have to treat this explicitly. We could use
     * memmove(), but we know that either `src` and `dest` do not overlap
     * at all, or fully. So we can be a bit more efficient here. */
    void *token_dest = coap_hdr_data_ptr(hdr);
    if (token_dest != token) {
        memcpy(token_dest, token, token_len);
    }

    return hdr_len;
}
