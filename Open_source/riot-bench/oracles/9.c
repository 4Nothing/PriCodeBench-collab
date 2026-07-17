int bcd_buf_from_str(const char *str, size_t len, void *dst, size_t dst_len)
{
    uint8_t *bcd = dst;
    memset(dst, 0, dst_len);
    dst_len *= 2;

    uint8_t i = 0;
    for (int j = len; j >= 0; --j) {
        if (i == dst_len) {
            return -ENOBUFS;
        }
        if (!_is_digit(str[j])) {
            continue;
        }
        uint8_t d = str[j] & 0xF;
        bcd[i >> 1] |= i & 1
                    ? d << 4
                    : d;
        ++i;
    }

    return (i & 1) + (i >> 1);
}