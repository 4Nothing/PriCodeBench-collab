uint32_t bcd_buf_to_u32(const void *src, size_t len)
{
    const uint8_t *bcd = src;
    uint32_t sum = 0;

    _BCD_CONVERT(sum, len);

    return sum;
}