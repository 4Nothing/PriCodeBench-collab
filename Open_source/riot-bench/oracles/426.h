static inline network_uint32_t byteorder_htonl(uint32_t v)
{
    network_uint32_t result = { .u32 = htobe32(v) };

    return result;
}

