static inline be_uint64_t byteorder_ltobll(le_uint64_t v)
{
    be_uint64_t result = { byteorder_swapll(v.u64) };

    return result;
}

