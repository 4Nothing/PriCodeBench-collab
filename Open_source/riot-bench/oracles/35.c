clif_attr_type_t clif_get_attr_type(const char *input, size_t input_len)
{
    assert(input);
    assert(input_len > 0);
    clif_attr_type_t ret = CLIF_ATTR_EXT;
    for (unsigned i = 0; i < ATTRS_NUMOF; i++) {
        if (input_len == _attr_to_size[i] &&
            !strncmp(input, _attr_to_str[i], input_len)) {
            ret = i;
            break;
        }
    }
    return ret;
}