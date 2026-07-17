static inline uint8_t ipv6_hdr_get_tc(const ipv6_hdr_t *hdr)
{
    return ((((hdr->v_tc_fl.u8[0]) & 0x0f) << 4) |
            ((hdr->v_tc_fl.u8[1] & 0xf0) >> 4));
}