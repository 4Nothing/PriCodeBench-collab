size_t coap_opt_put_uri_pathquery(uint8_t *buf, uint16_t *lastonum, const char *uri)
{
    size_t len;
    const char *query = strchr(uri, '?');
    uint16_t _lastonum = lastonum ? *lastonum : 0;

    if (query) {
        len = (query == uri) ? 0 : (query - uri - 1);
    } else {
        len = strlen(uri);
    }

    size_t bytes_out = coap_opt_put_string_with_len(buf, _lastonum,
                                                    COAP_OPT_URI_PATH,
                                                    uri, len, '/');
    if (query) {
        buf += bytes_out;
        bytes_out += coap_opt_put_uri_query(buf, COAP_OPT_URI_PATH, query + 1);
        _lastonum = COAP_OPT_URI_QUERY;
    }
    else if (bytes_out) {
        _lastonum = COAP_OPT_URI_PATH;
    }

    if (lastonum) {
        *lastonum = _lastonum;
    }

    return bytes_out;
}
