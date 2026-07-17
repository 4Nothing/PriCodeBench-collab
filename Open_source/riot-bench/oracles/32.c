ssize_t clif_decode_link(clif_t *link, clif_attr_t *attrs, unsigned attrs_len,
                         const char *buf, size_t maxlen)
{

    assert(buf);
    assert(link);

    char *pos;
    const char *end = buf + maxlen;
    clif_attr_t _dummy_attr;

    ssize_t size = clif_get_target(buf, maxlen, &pos);
    if (size < 0) {
        return CLIF_NOT_FOUND;
    }
    link->target = pos;
    link->target_len = size;
    link->attrs_len = 0;
    link->attrs = attrs;
    pos += size + 1; /* escape the '>' */

    DEBUG("Found target (%" PRIiSIZE "): %.*s\n", size, (unsigned)size,
          link->target);

    /* if there is no attr array iterate over the buffer, if not until all
     * the array is used */
    while ((!attrs && pos < end) || (attrs && link->attrs_len < attrs_len)) {
        clif_attr_t *attr = attrs ? &attrs[link->attrs_len] : &_dummy_attr;
        size = clif_get_attr(pos, end - pos, attr);
        if (size < 0) {
            break;
        }
        pos += size;
        link->attrs_len++;
    }

    return pos - buf;
}