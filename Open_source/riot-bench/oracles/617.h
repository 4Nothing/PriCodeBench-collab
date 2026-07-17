static inline ssize_t unicoap_options_add_uri_query_string(unicoap_options_t* options, char* query)
{
    return unicoap_options_add_uri_query(options, query, strlen(query));
}