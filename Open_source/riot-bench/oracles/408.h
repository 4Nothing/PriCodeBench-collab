static inline bool bit_check8(volatile uint8_t *ptr, uint8_t bit)
{
    return *((volatile uint8_t *)bitband_addr(ptr, bit));
}
