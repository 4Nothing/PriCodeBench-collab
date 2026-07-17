static inline bool bit_check32(volatile uint32_t *ptr, uint8_t bit)
{
    return *((volatile uint32_t *)bitband_addr(ptr, bit));
}
