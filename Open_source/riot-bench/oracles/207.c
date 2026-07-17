void ipv6_addr_init_iid(ipv6_addr_t *out, const uint8_t *iid, uint8_t bits)
{
    uint8_t unaligned_bits, bytes, pos;

    if (bits > 128) {
        bits = 128;
    }

    unaligned_bits = bits % 8;
    bytes = bits / 8;
    pos = (IPV6_ADDR_BIT_LEN / 8) - bytes;

    if (unaligned_bits) {
        uint8_t mask = 0xff << unaligned_bits;
        out->u8[pos - 1] &= mask;
        out->u8[pos - 1] |= (*iid & ~mask);
        iid++;
    }

    memcpy(&(out->u8[pos]), iid, bytes);
}