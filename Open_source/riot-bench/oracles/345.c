int uri_parser_process(uri_parser_result_t *result, const char *uri,
                       size_t uri_len)
{
    /* uri cannot be empty */
    if ((NULL == uri) || (uri[0] == '\0')) {
        return -1;
    }

    memset(result, 0, sizeof(*result));

    if (uri_parser_is_absolute(uri, uri_len)) {
        if (_parse_absolute(result, uri, uri + uri_len) != 0) {
            memset(result, 0, sizeof(*result));
            return -1;
        }
    }
    else {
        if (_parse_relative(result, uri, uri + uri_len) != 0) {
            memset(result, 0, sizeof(*result));
            return -1;
        }
    }

    return 0;
}

