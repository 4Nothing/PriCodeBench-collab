static inline ssize_t unicoap_options_copy_uri_path(const unicoap_options_t* options, char* path,
                                                    size_t capacity)
{
    assert(capacity > 0);
    *path = '/';
    path += 1;
    capacity -= 1;
    int res = unicoap_options_copy_values_joined(options, UNICOAP_OPTION_URI_PATH, (uint8_t*)path,
                                                 capacity, '/');
    return res < 0 ? res : res + 1;
}