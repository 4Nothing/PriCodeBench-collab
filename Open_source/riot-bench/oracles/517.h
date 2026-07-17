static inline uint16_t inet_csum(uint16_t sum, const uint8_t *buf, uint16_t len) {
    return inet_csum_slice(sum, buf, len, 0);
}