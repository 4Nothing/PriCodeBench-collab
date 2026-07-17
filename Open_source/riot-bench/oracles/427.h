static inline network_uint64_t byteorder_htonll(uint64_t v)
{
    network_uint64_t result = { .u64 = htobe64(v) };

    return result;
}

