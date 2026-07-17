static inline ssize_t unicoap_options_set_accept(unicoap_options_t* options,
                                                 unicoap_content_format_t format)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_ACCEPT, (uint32_t)format);
}