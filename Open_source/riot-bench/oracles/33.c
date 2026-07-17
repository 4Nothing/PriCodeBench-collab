ssize_t clif_encode_link(const clif_t *link, char *buf, size_t maxlen)
{
    assert(link);
    size_t pos = 0;
    ssize_t res = 0;

    res = clif_add_target_from_buffer(link->target, link->target_len, buf, maxlen);
    if (res < 0) {
        return res;
    }
    pos += res;

    for (unsigned i = 0; i < link->attrs_len; i++) {
        res = clif_add_attr(&link->attrs[i], buf ? &buf[pos] : NULL,
                             maxlen - pos);
        if (res <= 0) {
            return res;
        }
        pos += res;
    }
    return pos;
}