static inline uint8_t bcd_from_byte(uint8_t byte)
{
    /* ((byte / 10) << 4) | (byte % 10) */
    return byte + (6 * (byte / 10));
}
