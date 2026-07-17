static inline ssize_t unicoap_options_add_uri_path_component_string(unicoap_options_t* options,
                                                                    char* component)
{
    return unicoap_options_add_uri_path_component(options, component, strlen(component));
}