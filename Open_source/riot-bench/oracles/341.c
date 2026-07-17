int unicoap_options_add(unicoap_options_t* options, unicoap_option_number_t number,
                        const uint8_t* value, size_t value_size)
{
    unicoap_option_entry_t* opts = options->entries;
    assert(value_size <= UNICOAP_UINT_MAX);
    assert(opts->data);

    size_t count = options->option_count;
    if (count >= CONFIG_UNICOAP_OPTIONS_MAX) {
        OPTIONS_DEBUG("limit of %" PRIuSIZE " exceeded\n", (size_t)CONFIG_UNICOAP_OPTIONS_MAX);
        return -ENOBUFS;
    }

    size_t i = 0;
    while ((i < count) && (opts[i].number <= number)) {
        i += 1;
    }

    unicoap_option_entry_t* e = &opts[i];
    uint16_t delta = (i > 0) ? (number - opts[i - 1].number) : number;
    const ssize_t option_size = _option_size(delta, value_size);

    if (i == count) {
        size_t storage_size = options->storage_size + option_size;
        /* Option to be inserted is trailing option, can just add after last option */
        if (storage_size > options->storage_capacity) {
            OPTIONS_DEBUG("buf too small to insert opt " _UNICOAP_NEED_HAVE "\n",
                          storage_size, options->storage_capacity);
            return -ENOBUFS;
        }

        uint8_t* dest = (e->data = opts->data + options->storage_size);
        _write_option(&dest, delta, value, value_size);

        options->storage_size = storage_size;
        options->option_count += 1;
    }
    else {
        /* The successor's option delta will change */
        uint8_t* cursor = e->data;
        uint8_t length_nibble = DECODE_LENGTH_NIBBLE(*cursor);
        uint16_t new_delta = e->number - number;

        ssize_t diff = _option_size_diff(new_delta, DECODE_DELTA_NIBBLE(*cursor));
        ssize_t total_diff = option_size + diff;
        if (_shift_options(options, i, total_diff) < 0) {
            OPTIONS_DEBUG("storage too small for new option\n");
            return -ENOBUFS;
        }

        /* The size of the option (not just the value) grows if its new delta is wider than before.
         * If it is smaller, the entire option shrinks. */
        e->size += diff;

        /* If the options grows due to the delta field becoming wider, we extend the option in the
         * leading direction. Otherwise, we would need to move once to adjust for the delta width
         * and once again to accommodate the new option. We already created exactly enough space to
         * fit the new delta width AND the new option by calling
         * _shift_options(options, i, total_diff) above.
         *
         * Before:
         * ... value ] [ Nibbles (1B) | extended delta | extended length | value ]
         *            ^\_______ shift remainder ______>
         *            |
         *    New opt goes here
         *
         * After making space for new option + changed delta of successor option:
         * ... value ]                    [ Nibbles (1B) | extended delta | extended length | value]
         *            ^                   ^
         *            |                   |
         *    New opt goes here        e->data
         *
         * If successor opt delta width grows, e->data must move in leading direction
         * ... value ] ________________ [#######[ Nibbles old (1B) | extended old delta | ... ]
         *            ^       NEW       ^ (new) ^ (old)
         *            |                  \      |
         *    New opt goes here           \__ e->data
         *
         * If successor opt delta width shrinks, e->data must move in trailing direction
         * ... value ] ________________ [Nibbles old (1B) | extended old delta | ... ]
         *            ^    NEW   ^ (old) ^ (new)
         *            |          |      /
         *    New opt goes here  e->data
         */
        e->data -= diff;
        _shift_option_entries(options, i, 1);

        e->data = cursor;
        _write_option(&cursor, delta, value, value_size);
        _write_head_partial(&cursor, new_delta, length_nibble);
    }

    e->number = number;
    e->size = option_size;
    return 0;
}