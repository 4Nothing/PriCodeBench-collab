static inline int cib_get(cib_t *__restrict cib)
{
    if (cib_avail(cib)) {
        return (int)(cib->read_count++ & cib->mask);
    }

    return -1;
}
