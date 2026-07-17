static inline void unicoap_options_iterator_init(unicoap_options_iterator_t* iterator,
                                                 unicoap_options_t* options)
{
    assert(iterator);
    assert(options);
    assert(options->entries->data);
    iterator->options = options;
    iterator->index = 0;
}