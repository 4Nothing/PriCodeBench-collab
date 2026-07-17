static inline be_uint16_t byteorder_ltobs(le_uint16_t v)
{
    be_uint16_t result = { byteorder_swaps(v.u16) };

    return result;
}

