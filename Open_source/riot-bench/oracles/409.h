static inline void bit_clear16(volatile uint16_t *ptr, uint8_t bit)
{
    *((volatile uint16_t *)bitband_addr(ptr, bit)) = 0;
}
