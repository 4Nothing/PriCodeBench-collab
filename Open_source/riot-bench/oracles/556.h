static inline void ipv6_hdr_set_version(ipv6_hdr_t *hdr)
{
    hdr->v_tc_fl.u8[0] &= 0x0f;
    hdr->v_tc_fl.u8[0] |= 0x60;
}