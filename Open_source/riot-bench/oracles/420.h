static inline le_uint16_t byteorder_btols(be_uint16_t v)
{
    le_uint16_t result = { byteorder_swaps(v.u16) };

    return result;
}

