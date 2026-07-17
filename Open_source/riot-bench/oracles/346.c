int uri_parser_process_string(uri_parser_result_t *result, const char *uri)
{
    return uri_parser_process(result, uri, strlen(uri));
}

