static inline uint16_t byteorder_ntohs(network_uint16_t v)
{
    return be16toh(v.u16);
}

