static inline uint32_t div_u32_by_15625div512(uint32_t val)
{
    return ((uint64_t)(val) * DIV_H_INV_15625_32) >> (DIV_H_INV_15625_SHIFT + 32 - 9);
}
