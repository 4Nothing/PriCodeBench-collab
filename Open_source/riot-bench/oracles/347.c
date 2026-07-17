int uri_parser_split_query(const uri_parser_result_t *uri,
                           uri_parser_query_param_t *params,
                           size_t params_len)
{
    const char *query_end;
    unsigned idx = 0;

    assert(uri);
    assert(params);

    if ((uri->query == NULL) || (uri->query_len == 0) || (params_len == 0)) {
        return 0;
    }
    assert(params[0].name == 0);
    assert(params[0].name_len == 0);
    assert(params[0].value == 0);
    assert(params[0].value_len == 0);
    query_end = uri->query + uri->query_len;
    params[0].name = uri->query;
    for (const char *c = uri->query; c < query_end; c++) {
        switch (*c) {
            case '#':
            case '&':
                if (params[idx].value == NULL) {
                    /* we should have picked up a parameter value by now */
                    return -1;
                }
                params[idx].value_len = c - params[idx].value;
                if (*c == '#') {
                    /* we've reached the end of the query string, next comes an
                     * anchor, enforce end of loop
                     * XXX: can be removed when uri_parser has anchor support */
                    c = query_end;
                }
                else if ((idx + 1) < params_len) {
                    /* c is an ampersand (&), so mark the next char as the next
                     * parameter's name name */
                    params[++idx].name = c + 1U;
                    assert(params[idx].name_len == 0);
                }
                else {
                    /* c is an ampersand (&), but we exceeded param_len.
                     * Return -2 as per doc */
                    return -2;
                }
                break;
            case '=':
                /* params[idx].value != NULL picks up duplicate = in query
                 * parameter */
                if ((params[idx].name == NULL) || params[idx].value != NULL) {
                    /* we should have picked up a parameter name by now */
                    return -1;
                }
                params[idx].name_len = c - params[idx].name;
                /* pick next char as start of value */
                params[idx].value = c + 1U;
                /* make sure the precondition on params is met */
                assert(params[idx].value_len == 0);
                break;
            default:
                break;
        }
    }
    if ((uri->query != query_end) && (params[idx].value == NULL)) {
        /* we should have picked up a parameter value by now */
        return -1;
    }
    /* set final value_len */
    params[idx].value_len = query_end - params[idx].value;
    return idx + 1;
}
