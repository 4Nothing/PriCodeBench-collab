static inline size_t coap_opt_put_uri_path(uint8_t *buf, uint16_t lastonum,
                                           const char *uri)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_URI_PATH, uri, '/');
}
