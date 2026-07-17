static inline uint64_t div_u64_by_15625(uint64_t val)
{
    if (val > 16383999997ull) {
        return (_div_mulhi64(DIV_H_INV_15625_64, val) >> DIV_H_INV_15625_SHIFT);
    }
    return (val * DIV_H_INV_15625_32) >> (DIV_H_INV_15625_SHIFT + 32);
}
