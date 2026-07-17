static inline le_uint64_t byteorder_htolll(uint64_t v)
{
    le_uint64_t result = { .u64 = htole64(v) };

    return result;
}

