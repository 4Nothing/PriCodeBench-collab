static inline int cib_put(cib_t *__restrict cib)
{
    unsigned int avail = cib_avail(cib);

    /* We use a signed compare, because the mask is -1u for an empty CIB. */
    if ((int)avail <= (int)cib->mask) {
        return (int)(cib->write_count++ & cib->mask);
    }

    return -1;
}
