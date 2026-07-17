unsigned bf_popcnt(const uint8_t field[], size_t size)
{
    unsigned bytes = size >> 3;
    unsigned bits = size & 0x7;
    unsigned mask = ~((1U << (8 - bits)) - 1);

    unsigned count = 0;
    for (unsigned i = 0; i < bytes; ++i) {
        count += bitarithm_bits_set(field[i]);
    }

    if (bits) {
        count += bitarithm_bits_set(field[bytes] & mask);
    }

    return count;
}