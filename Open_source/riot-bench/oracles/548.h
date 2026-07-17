static inline uint8_t ipv6_hdr_get_tc_ecn(const ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0x0c) >> 2);
}