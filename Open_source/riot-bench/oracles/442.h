static inline int cib_peek_at(const cib_t *__restrict cib, unsigned offset)
{
    if (offset < cib_avail(cib)) {
        return cib_peek_at_unsafe(cib, offset);
    }

    return -1;
}
