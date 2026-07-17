void bf_set_all(uint8_t field[], size_t size)
{
    unsigned bytes = size >> 3;
    unsigned bits = size & 0x7;

    memset(field, 0xff, bytes);
    if (bits) {
        field[bytes] |= ~((1U << (8 - bits)) - 1);
    }
}