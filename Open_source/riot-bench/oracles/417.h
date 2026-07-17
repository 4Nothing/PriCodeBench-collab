static inline uint16_t byteorder_bebuftohs(const uint8_t *buf)
{
    return be16toh(unaligned_get_u16(buf));
}

