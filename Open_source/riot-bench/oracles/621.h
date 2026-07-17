static inline ssize_t unicoap_options_get_accept(const unicoap_options_t* options,
                                                 unicoap_content_format_t* format)
{
    return unicoap_options_get_uint16(options, UNICOAP_OPTION_ACCEPT, (uint16_t*)format);
}