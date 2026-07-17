static inline le_uint64_t byteorder_btolll(be_uint64_t v)
{
    le_uint64_t result = { byteorder_swapll(v.u64) };

    return result;
}

