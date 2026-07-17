bool coap_find_uri_query(coap_pkt_t *pkt, const char *key, const char **value, size_t *len)
{
    uint8_t *opt_pos = NULL;

    while (1) {
        int len_query;
        const void *key_data = coap_iterate_option(pkt, COAP_OPT_URI_QUERY,
                                                   &opt_pos, &len_query);
        if (key_data == NULL) {
            return false;
        }

        const char *separator = memchr(key_data, '=', len_query);
        size_t len_key = separator
                       ? (separator - (char *)key_data)
                       : len_query;

        if (memcmp(key, key_data, len_key)) {
            continue;
        }

        if (value == NULL) {
            return true;
        }

        assert(len);
        if (separator) {
            *value = separator + 1;
            *len = len_query - len_key - 1;
        } else {
            *value = NULL;
            *len   = 0;
        }
        return true;
    }

    return false;
}
