int unicoap_options_set(unicoap_options_t* options, unicoap_option_number_t number,
                        const uint8_t* value, size_t value_size)
{
    unicoap_option_entry_t* opts = options->entries;
    assert(value_size <= UNICOAP_UINT_MAX);
    assert(opts->data);

    int i = _find_option_index(options, number);
    if (i < 0) {
        return unicoap_options_add(options, number, value, value_size);
    }
    else {
        unicoap_option_entry_t* e = &opts[i];
        uint16_t delta = (i > 0) ? (number - opts[i - 1].number) : number;

        size_t option_size = _option_size(delta, value_size);
        if (_shift_options(options, i + 1, (int)option_size - (int)e->size) < 0) {
            OPTIONS_DEBUG("storage too small for new option value\n");
            return -ENOBUFS;
        }

        e->size = option_size;
        uint8_t* dest = e->data;
        _write_option(&dest, delta, value, value_size);
    }
    return 0;
}