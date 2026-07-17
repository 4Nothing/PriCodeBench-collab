static inline void bit_clear32(volatile uint32_t *ptr, uint8_t bit)
{
    *((volatile uint32_t *)bitband_addr(ptr, bit)) = 0;
}
