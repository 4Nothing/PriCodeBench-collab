static inline void ipv6_hdr_set_tc_dscp(ipv6_hdr_t *hdr, uint8_t dscp)
{
    hdr->v_tc_fl.u8[0] &= 0xfc;
    hdr->v_tc_fl.u8[0] |= (0x03 & (dscp >> 4));
    hdr->v_tc_fl.u8[1] &= 0x0f;
    hdr->v_tc_fl.u8[1] |= (0xf0 & (dscp << 4));
}