static inline void bit_set8(volatile uint8_t *ptr, uint8_t bit)
{
    *((volatile uint8_t *)bitband_addr(ptr, bit)) = 1;
}
