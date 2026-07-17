static inline uint8_t bcd_to_byte(uint8_t bcd)
{
    /* == (10 * (bcd >> 4)) + (bcd & 0xf) */
    return bcd - (6 * (bcd >> 4));
}
