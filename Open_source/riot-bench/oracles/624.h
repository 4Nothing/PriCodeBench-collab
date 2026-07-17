static inline ssize_t unicoap_options_get_next_uri_query(unicoap_options_iterator_t* iterator,
                                                         const char** query)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_URI_QUERY,
                                              (const uint8_t**)query);
}