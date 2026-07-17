ssize_t iolist_to_buffer(const iolist_t *iolist, void *buf, size_t len)
{
    char *dst = buf;

    while (iolist) {
        if (iolist->iol_len > len) {
            return -ENOBUFS;
        }
        memcpy(dst, iolist->iol_base, iolist->iol_len);
        len -= iolist->iol_len;
        dst += iolist->iol_len;
        iolist = iolist->iol_next;
    }

    return (uintptr_t)dst - (uintptr_t)buf;
}
