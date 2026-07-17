const void *memchk(const void *data, uint8_t c, size_t len)
{
    const uint8_t *end = (uint8_t *)data + len;
    for (const uint8_t *d = data; d != end; ++d) {
        if (c != *d) {
            return d;
        }
    }

    return NULL;
}