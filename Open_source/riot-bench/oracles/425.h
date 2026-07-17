static inline le_uint16_t byteorder_htols(uint16_t v)
{
    le_uint16_t result = { .u16 = htole16(v) };

    return result;
}

