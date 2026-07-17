static inline void ipv6_hdr_set_tc_ecn(ipv6_hdr_t *hdr, uint8_t ecn)
{
    hdr->v_tc_fl.u8[0] &= 0xf3;
    hdr->v_tc_fl.u8[0] |= (0x0c & (ecn << 2));
}