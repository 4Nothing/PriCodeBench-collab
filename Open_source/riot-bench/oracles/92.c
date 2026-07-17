int dns_cache_query(const char *domain_name, void *addr_out, int family)
{
    int res = 0;
    uint32_t now = ztimer_now(ZTIMER_MSEC) / MS_PER_SEC;
    uint32_t hash = _hash(domain_name, strlen(domain_name));
    uint8_t addr_len = _addr_len(family);

    mutex_lock(&cache_mutex);
    for (unsigned i = 0; i < CONFIG_DNS_CACHE_SIZE; ++i) {
        /* empty slot */
        if (_is_empty(i)) {
            continue;
        }
        /* TTL expired - invalidate slot */
        if (now > cache[i].expires) {
            DEBUG("dns_cache[%u] expired\n", i);
            _set_empty(i);
            continue;
        }
        /* check if hash and length match */
        if (cache[i].hash == hash && (!addr_len || addr_len == _get_len(i))) {
            DEBUG("dns_cache[%u] hit\n", i);
            memcpy(addr_out, &cache[i].addr, _get_len(i));
            res = _get_len(i);
            break;
        }
    }
    if (res == 0) {
        DEBUG("dns_cache miss\n");
    }
    mutex_unlock(&cache_mutex);
    return res;
}