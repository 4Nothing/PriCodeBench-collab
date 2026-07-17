ssize_t clif_attr_type_to_str(clif_attr_type_t type, const char **str)
{
    if (type < ATTRS_NUMOF) {
        *str = _attr_to_str[type];
        return _attr_to_size[type];
    }
    return CLIF_NOT_FOUND;
}