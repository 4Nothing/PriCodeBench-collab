static inline void ipv6_hdr_set_fl(ipv6_hdr_t *hdr, uint32_t fl)
{
    hdr->v_tc_fl.u8[1] &= 0xf0;
    hdr->v_tc_fl.u8[1] |= (0x0f & (byteorder_htonl(fl).u8[1]));
    hdr->v_tc_fl.u16[1] = byteorder_htonl(fl).u16[1];
}