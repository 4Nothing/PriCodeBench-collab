unsigned bitarithm_bits_set(unsigned v)
{
    unsigned c; /* c accumulates the total bits set in v */

    for (c = 0; v; c++) {
        v &= v - 1; /* clear the least significant bit set */
    }

    return c;
}
