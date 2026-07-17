static inline bool bf_isset(const uint8_t field[], size_t idx)
{
    return (field[idx / 8] & (1u << (7 - (idx % 8))));
}