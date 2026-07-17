uint32_t crc32(const void *buf, size_t size)
{
    const uint8_t *p = buf;
    uint32_t crc = ~0U;

    for (unsigned i = 0; i < size; ++i) {
        crc ^= *p++;
        crc = crc32_for_byte(crc);

    }

    return ~crc;
}