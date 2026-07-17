static inline void cib_init(cib_t *__restrict cib, unsigned int size)
{
    /* check if size is a power of 2 by comparing it to its complement */
    assert(!(size & (size - 1)));

    cib_t c = CIB_INIT(size);

    *cib = c;
}
