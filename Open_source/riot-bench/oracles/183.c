gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_update(uint8_t id, const ipv6_addr_t *prefix,
                                                uint8_t prefix_len, uint16_t ltime,
                                                bool comp)
{
    if ((id >= GNRC_SIXLOWPAN_CTX_SIZE) || (prefix_len == 0)) {
        return NULL;
    }

    mutex_lock(&_ctx_mutex);

    _ctxs[id].ltime = ltime;

    if (ltime == 0) {
        comp = false;
    }

    if (prefix_len > IPV6_ADDR_BIT_LEN) {
        _ctxs[id].prefix_len = IPV6_ADDR_BIT_LEN;
    }
    else {
        _ctxs[id].prefix_len = prefix_len;
    }

    _ctxs[id].flags_id = (comp) ? (GNRC_SIXLOWPAN_CTX_FLAGS_COMP | id) : id;

    if (!ipv6_addr_equal(&(_ctxs[id].prefix), prefix)) {
        ipv6_addr_set_unspecified(&(_ctxs[id].prefix));
        ipv6_addr_init_prefix(&(_ctxs[id].prefix), prefix, _ctxs[id].prefix_len);
    }
    DEBUG("6lo ctx: update context (%u, %s/%" PRIu8 "), lifetime: %" PRIu16 " min\n",
          id, ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
          _ctxs[id].prefix_len, _ctxs[id].ltime);
    _ctx_inval_times[id] = ltime + _current_minute();

    mutex_unlock(&_ctx_mutex);
    return &(_ctxs[id]);
}