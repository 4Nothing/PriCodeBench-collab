static inline ssize_t unicoap_options_copy_uri_queries(const unicoap_options_t* options,
                                                       char* queries, size_t capacity)
{
    return unicoap_options_copy_values_joined(options, UNICOAP_OPTION_URI_QUERY, (uint8_t*)queries,
                                              capacity, '&');
}