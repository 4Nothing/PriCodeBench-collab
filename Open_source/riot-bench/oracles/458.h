static inline ssize_t coap_build_hdr(coap_udp_hdr_t *hdr, unsigned type, const void *token,
                                     size_t token_len, unsigned code, uint16_t id)
{
    size_t fingers_crossed_size = sizeof(*hdr) + token_len;

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        /* With RFC 8974, we have an additional extended TKL
         * field of 0-4 bytes in length */
        fingers_crossed_size += 4;
    }

    return coap_build_udp_hdr(hdr, fingers_crossed_size, type, token, token_len, code, id);
}
