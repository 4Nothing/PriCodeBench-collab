gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_lookup_id(uint8_t id)
{
    if (id >= GNRC_SIXLOWPAN_CTX_SIZE) {
        return NULL;
    }

    mutex_lock(&_ctx_mutex);

    if (_valid(id)) {
        DEBUG("6lo ctx: found context (%u, %s/%" PRIu8 ")\n", id,
              ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
              _ctxs[id].prefix_len);
        mutex_unlock(&_ctx_mutex);
        return &(_ctxs[id]);
    }

    mutex_unlock(&_ctx_mutex);

    return NULL;
}