static inline uint64_t byteorder_ntohll(network_uint64_t v)
{
    return be64toh(v.u64);
}

