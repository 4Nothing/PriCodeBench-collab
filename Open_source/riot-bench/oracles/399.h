static inline void bf_or(uint8_t out[], const uint8_t a[], const uint8_t b[], size_t len)
{
    len = (len + 7) / 8;
    while (len--) {
        out[len] = a[len] | b[len];
    }
}