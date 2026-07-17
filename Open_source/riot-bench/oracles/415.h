static inline unsigned bitarithm_msb(unsigned v)
{
#if defined(BITARITHM_HAS_CLZ)
    return 8 * sizeof(v) - __builtin_clz(v) - 1;
#elif ARCH_32_BIT
    return bitarith_msb_32bit_no_native_clz(v);
#else
    unsigned r = 0;
    while (v >>= 1) { /* unroll for more speed... */
        ++r;
    }
    return r;
#endif
}
