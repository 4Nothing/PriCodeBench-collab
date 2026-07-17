uint8_t ipv6_addr_match_prefix(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    uint8_t prefix_len = 0;

    if ((a == NULL) || (b == NULL)) {
        return 0;
    }

    if (a == b) {
        return 128;
    }

    for (int i = 0; i < 16; i++) {
        uint8_t xor = a->u8[i] ^ b->u8[i];
        if (xor) {
            /* if bytes aren't equal count matching leading bits */
            prefix_len += bitarithm_clzb(xor);
            break;
        }
        else {
            /* if bytes are equal add 8 */
            prefix_len += 8;
        }
    }

    return prefix_len;
}