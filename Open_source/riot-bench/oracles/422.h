static inline void byteorder_htobebufs(uint8_t *buf, uint16_t val)
{
    val = htobe16(val);
    memcpy(buf, &val, sizeof(val));
}

