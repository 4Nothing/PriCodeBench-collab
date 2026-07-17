static inline void tsrb_init(tsrb_t *rb, uint8_t *buffer, unsigned bufsize)
{
    /* make sure bufsize is a power of two.
     * http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
     */
    assert((bufsize != 0) && ((bufsize & (~bufsize + 1)) == bufsize));

    rb->buf = buffer;
    rb->size = bufsize;
    rb->reads = 0;
    rb->writes = 0;
}