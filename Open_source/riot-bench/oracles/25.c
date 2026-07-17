bool bloom_check(bloom_t *bloom, const uint8_t *buf, size_t len)
{
    for (size_t n = 0; n < bloom->k; n++) {
        uint32_t hash = bloom->hash[n](buf, len);

        if (!(bf_isset(bloom->a, (hash % bloom->m)))) {
            return false;
        }
    }

    return true; /* ? */
}
