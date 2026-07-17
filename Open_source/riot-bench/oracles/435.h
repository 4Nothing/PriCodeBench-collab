static inline uint32_t byteorder_ntohl(network_uint32_t v)
{
    return be32toh(v.u32);
}

