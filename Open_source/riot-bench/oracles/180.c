gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_lookup_addr(const ipv6_addr_t *addr)
{
    uint8_t best = 0;
    gnrc_sixlowpan_ctx_t *res = NULL;

    mutex_lock(&_ctx_mutex);

    for (unsigned int id = 0; id < GNRC_SIXLOWPAN_CTX_SIZE; id++) {
        if (_valid(id)) {
            uint8_t match = ipv6_addr_match_prefix(&_ctxs[id].prefix, addr);

            if ((_ctxs[id].prefix_len <= match) && (match > best)) {
                best = match;
                res = &(_ctxs[id]);
            }
        }
    }

    mutex_unlock(&_ctx_mutex);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (res != NULL) {
            DEBUG("6lo ctx: found context (%u, %s/%" PRIu8 ") ",
                (res->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK),
                ipv6_addr_to_str(ipv6str, &res->prefix, sizeof(ipv6str)),
                res->prefix_len);
            DEBUG("for address %s\n", ipv6_addr_to_str(ipv6str, addr, sizeof(ipv6str)));
        }
    }

    return res;
}