ssize_t clif_get_attr(const char *input, size_t input_len, clif_attr_t *attr)
{
    assert(input);
    assert(attr);
    const char *pos = input;
    const char *end = input + input_len;
    unsigned quotes = 0;
    bool scan_value = false;

    /* initialize attr */
    attr->value = NULL;
    attr->key = NULL;
    attr->key_len = 0;
    attr->value_len = 0;

    if (input_len == 0) {
        return CLIF_NOT_FOUND;
    }

    /* an attribute should start with the separator */
    if (*pos != LF_ATTR_SEPARATOR_C) {
        DEBUG("Attribute should start with separator, found %c\n", *pos);
        return CLIF_NOT_FOUND;
    }
    pos++;
    attr->key = pos;

    /* iterate over key */
    while (pos < end) {
        if (*pos == LF_ATTR_SEPARATOR_C || *pos == LF_LINK_SEPARATOR_C) {
            /* key ends, no value */
            attr->key_len = pos - attr->key;
            break;
        }
        if (*pos == LF_ATTR_VAL_SEPARATOR_C) {
            /* key ends, has value */
            attr->key_len = pos - attr->key;
            /* check if the value is quoted and prepare pointer for value scan */
            pos++;
            if (pos == end) {
                /* found attribute-value separator but no value */
                return CLIF_NOT_FOUND;
            }
            else if (*pos == '"') {
                quotes++;
                pos++;
            }
            attr->value = (char *)pos;
            scan_value = true;
            break;
        }
        pos++;
    }

    if (scan_value) {
        /* iterate over value */
        while (pos < end) {
            if (quotes == 1) {
                /* we can safely access *(pos - 1) because at least one
                 * character was detected to get to this point */
                if (*pos == '"' && *(pos - 1) != '\\') {
                    /* found unescaped quote */
                    attr->value_len = pos - attr->value;
                    quotes++;
                    pos++;
                    break;
                }
            }
            else {
                if (*pos == '"') {
                    /* not valid */
                    return CLIF_NOT_FOUND;
                }

                if (*pos == LF_ATTR_SEPARATOR_C || *pos == LF_LINK_SEPARATOR_C) {
                    /* value ends */
                    attr->value_len = pos - attr->value;
                    break;
                }
            }
            pos++;
        }

        if (!attr->value_len) {
            DEBUG("Empty value found");
            return CLIF_NOT_FOUND;
        }
    }
    else {
        /* buffer exhausted and no special character found, calculate length of
        * attribute and exit */
        attr->key_len = pos - attr->key;
    }

    /* either the value is unquoted (0) or quoted (2) */
    if (quotes % 2U) {
        DEBUG("Incorrect number of unescaped quotes found: %d\n", quotes);
        return CLIF_NOT_FOUND;
    }

    return pos - input;
}