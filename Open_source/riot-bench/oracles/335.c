uint16_t ucrc16_calc_be(const uint8_t *buf, size_t len, uint16_t poly,
                        uint16_t seed)
{
    assert(buf != NULL);
    for (unsigned c = 0; c < len; c++, buf++) {
        uint32_t tmp = seed ^ (*buf << (UINT16_BIT_SIZE - UINT8_BIT_SIZE));
        for (unsigned i = 0; i < UINT8_BIT_SIZE; i++) {
            tmp = LEFTMOST_BIT_SET(tmp) ? ((tmp << 1) ^ poly) : (tmp << 1);
        }
        seed = tmp & UINT16_MAX;
    }
    return seed;
}
