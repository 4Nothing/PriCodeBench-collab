ssize_t clif_add_link_separator(char *buf, size_t maxlen)
{
    if (!buf) {
        return 1;
    }

    if (maxlen < 1) {
        return CLIF_NO_SPACE;
    }

    *buf = LF_LINK_SEPARATOR_C;
    return 1;
}