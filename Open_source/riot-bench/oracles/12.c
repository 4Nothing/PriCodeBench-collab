void bf_clear_all(uint8_t field[], size_t size)
{
    unsigned bytes = size >> 3;
    unsigned bits = size & 0x7;

    memset(field, 0, bytes);
    if (bits) {
        field[bytes] &= ((1U << (8 - bits)) - 1);
    }
}