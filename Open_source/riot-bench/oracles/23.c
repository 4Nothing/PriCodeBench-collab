uint8_t bitarithm_bits_set_u32(uint32_t v)
{
    uint8_t c;

    for (c = 0; v; c++) {
        v &= v - 1; /* clear the least significant bit set */
    }
    return c;
}
