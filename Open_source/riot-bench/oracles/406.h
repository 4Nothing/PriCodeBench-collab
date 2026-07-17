static inline bool bit_check16(volatile uint16_t *ptr, uint8_t bit)
{
    return *((volatile uint16_t *)bitband_addr(ptr, bit));
}
