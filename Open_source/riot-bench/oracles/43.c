int coap_iterate_uri_query(coap_pkt_t *pkt, void **opt_pos,
                           char *key, size_t key_len_max,
                           char *value, size_t value_len_max)
{
    int len;
    void *key_data = coap_iterate_option(pkt, COAP_OPT_URI_QUERY,
                                         (uint8_t **)opt_pos, &len);
    if (!key_data) {
        return 0; /* No key found */
    }

    const char *value_data = memchr(key_data, '=', len);

    size_t key_len, value_len;

    if (value_data) {
        key_len = (uintptr_t)value_data - (uintptr_t)key_data;
        value_data += 1;
        value_len = len - key_len - 1;
    } else {
        key_len = len;
        value_data = NULL;
        value_len = 0;

        if (value && value_len_max) {
            value[0] = 0;
        }
    }

    if (key_len >= key_len_max) {
        return -E2BIG;
    }
    memcpy(key, key_data, key_len);
    key[key_len] = 0;

    if (!value_data) {
        return 1; /* Key was found but no values */
    }
    if (!value) {
        return 2; /* Key and values found */
    }

    if (value_len >= value_len_max) {
        return -E2BIG;
    }
    memcpy(value, value_data, value_len);
    value[value_len] = 0;
    return 2; /* Key and values found */
}
