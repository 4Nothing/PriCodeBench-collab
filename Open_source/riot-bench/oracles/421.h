static inline void byteorder_htobebufl(uint8_t *buf, uint32_t val)
{
    val = htobe32(val);
    memcpy(buf, &val, sizeof(val));
}

