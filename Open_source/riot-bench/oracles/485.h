static inline uint64_t div_u64_by_1000000(uint64_t val)
{
    return div_u64_by_15625(val) >> 6;
}
