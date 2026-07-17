static inline uint64_t div_u64_by_15625div512(uint64_t val)
{
    /*
     * This saves around 1400 bytes of ROM on Cortex-M platforms (both ARMv6 and
     * ARMv7) from avoiding linking against __aeabi_uldivmod and related helpers
     */
    if (val > 16383999997ull) {
        /* this would overflow 2^64 in the multiplication that follows, need to
         * use the long version */
        return (_div_mulhi64(DIV_H_INV_15625_64, val) >> (DIV_H_INV_15625_SHIFT - 9));
    }
    return (val * DIV_H_INV_15625_32) >> (DIV_H_INV_15625_SHIFT + 32 - 9);
}
