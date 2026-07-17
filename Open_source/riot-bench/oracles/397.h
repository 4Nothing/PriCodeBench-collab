static inline void bf_inv(uint8_t out[], const uint8_t a[], size_t len)
{
    len = (len + 7) / 8;
    while (len--) {
        out[len] = ~a[len];
    }
}