static inline le_uint32_t byteorder_btoll(be_uint32_t v)
{
    le_uint32_t result = { byteorder_swapl(v.u32) };

    return result;
}

