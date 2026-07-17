static inline void bf_unset(uint8_t field[], size_t idx)
{
    field[idx / 8] &= ~(1u << (7 - (idx % 8)));
}