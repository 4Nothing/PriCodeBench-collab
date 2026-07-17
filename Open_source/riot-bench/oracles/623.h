static inline ssize_t
unicoap_options_get_next_uri_path_component(unicoap_options_iterator_t* iterator,
                                            const char** component)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_URI_PATH,
                                              (const uint8_t**)component);
}