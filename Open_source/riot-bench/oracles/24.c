void bloom_add(bloom_t *bloom, const uint8_t *buf, size_t len)
{
    for (size_t n = 0; n < bloom->k; n++) {
        uint32_t hash = bloom->hash[n](buf, len);
        bf_set(bloom->a, (hash % bloom->m));
    }
}
