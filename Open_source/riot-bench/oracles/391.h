static inline void atomic_store_u16(volatile uint16_t *dest, uint16_t val)
{
    __atomic_store_2(dest, val, __ATOMIC_SEQ_CST);
}
