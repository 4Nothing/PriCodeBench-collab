static inline uint32_t byteorder_ltohl(le_uint32_t v)
{
    return le32toh(v.u32);
}

