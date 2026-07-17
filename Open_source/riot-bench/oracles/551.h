static inline bool ipv6_hdr_is(const ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0xf0) == 0x60);
}