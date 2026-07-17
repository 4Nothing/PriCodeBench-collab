static inline network_uint16_t byteorder_htons(uint16_t v)
{
    network_uint16_t result = { .u16 = htobe16(v) };

    return result;
}

