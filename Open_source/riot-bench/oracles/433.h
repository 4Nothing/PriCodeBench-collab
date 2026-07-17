static inline uint64_t byteorder_ltohll(le_uint64_t v)
{
    return le64toh(v.u64);
}

