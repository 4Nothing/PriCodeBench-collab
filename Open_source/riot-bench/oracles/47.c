ssize_t coap_opt_add_uri_query2(coap_pkt_t *pkt, const char *key, size_t key_len,
                                const char *val, size_t val_len)
{
    assert(pkt);
    assert(key);
    assert(key_len);
    assert(!val_len || (val && val_len));

    char qs[CONFIG_NANOCOAP_QS_MAX];
    /* length including '=' */
    size_t qs_len = key_len + ((val && val_len) ? (val_len + 1) : 0);

    /* test if the query string fits */
    if (qs_len > CONFIG_NANOCOAP_QS_MAX) {
        return -1;
    }

    memcpy(&qs[0], key, key_len);
    if (val && val_len) {
        qs[key_len] = '=';
        memcpy(&qs[key_len + 1], val, val_len);
    }

    return _add_opt_pkt(pkt, COAP_OPT_URI_QUERY, (uint8_t *)qs, qs_len);
}
