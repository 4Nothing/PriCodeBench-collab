static inline int unicoap_options_remove(unicoap_options_t* options, unicoap_option_number_t number)
{
    /* reuse unicoap_options_remove_all, helps decrease binary size */
    return unicoap_options_remove_all(options, number);
}