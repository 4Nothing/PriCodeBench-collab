int bcd_buf_from_u32(uint32_t val, void *dst, size_t len)
{
    uint8_t *tgt = dst;
    uint8_t hex = 0;
    uint8_t idx = 0;

    memset(dst, 0, len);
    len *= 2;

    do {
        hex <<= 4;
        hex += val % 10;
        val /= 10;
        if (++idx % 2 == 0) {
            *tgt++ = swap_nibbles(hex);
            hex = 0;
        }
    } while (val && idx <= len);

    if (idx > len) {
        return -ENOBUFS;
    }

    if (idx % 2) {
        *tgt++ = hex;
    }

    return (uintptr_t)tgt - (uintptr_t)dst;
}