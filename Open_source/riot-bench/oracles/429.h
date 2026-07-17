static inline be_uint32_t byteorder_ltobl(le_uint32_t v)
{
    be_uint32_t result = { byteorder_swapl(v.u32) };

    return result;
}

