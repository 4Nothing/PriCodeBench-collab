static inline void bit_set32(volatile uint32_t *ptr, uint8_t bit)
{
    *((volatile uint32_t *)bitband_addr(ptr, bit)) = 1;
}
