void dns_cache_add(const char *domain_name, const void *addr_out,
                        int addr_len, uint32_t ttl)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC) / MS_PER_SEC;
    uint32_t hash = _hash(domain_name, strlen(domain_name));
    uint32_t oldest = ttl;
    int idx = -1;

    assert(addr_len == 4 || addr_len == 16);
    DEBUG("dns_cache: lifetime of %s is %"PRIu32" s\n", domain_name, ttl);

    mutex_lock(&cache_mutex);
    /* iterate even if TTL = 0 just in case we need to expire */
    for (unsigned i = 0; i < CONFIG_DNS_CACHE_SIZE; ++i) {
        if (ttl && (now > cache[i].expires || _is_empty(i))) {
            _add_entry(i, hash, addr_out, addr_len, now + ttl);
            goto exit;
        }
        if (cache[i].hash == hash && _get_len(i) == addr_len) {
            DEBUG("dns_cache[%u] update ttl\n", i);
            if (ttl) {
                cache[i].expires = now + ttl;
            }
            else {
                /* put one second into past so that it is immediately expired */
                cache[i].expires = now - 1;
            }
            goto exit;
        }
        uint32_t _ttl = cache[i].expires - now;
        if (_ttl < oldest) {
            oldest = _ttl;
            idx = i;
        }
    }

    if (ttl && idx >= 0) {
        DEBUG("dns_cache: evict first entry to expire\n");
        _add_entry(idx, hash, addr_out, addr_len, now + ttl);
    }
exit:
    mutex_unlock(&cache_mutex);
}