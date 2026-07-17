static inline void bit_set16(volatile uint16_t *ptr, uint8_t bit)
{
    *((volatile uint16_t *)bitband_addr(ptr, bit)) = 1;
}
