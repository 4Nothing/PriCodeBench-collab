static inline void bf_set(uint8_t field[], size_t idx)
{
    field[idx / 8] |= (1u << (7 - (idx % 8)));
}