static inline le_uint32_t byteorder_htoll(uint32_t v)
{
    le_uint32_t result = { .u32 = htole32(v) };

    return result;
}

