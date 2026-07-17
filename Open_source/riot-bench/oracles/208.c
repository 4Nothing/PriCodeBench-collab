void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits)
{
    uint8_t bytes;

    if (bits > 128) {
        bits = 128;
    }

    bytes = bits / 8;

    memcpy(out, prefix, bytes);

    if (bits % 8) {
        uint8_t mask = 0xff << (8 - (bits - (bytes * 8)));

        out->u8[bytes] &= ~mask;
        out->u8[bytes] |= (prefix->u8[bytes] & mask);
    }
}