static inline uint32_t byteorder_bebuftohl(const uint8_t *buf)
{
    return be32toh(unaligned_get_u32(buf));
}

