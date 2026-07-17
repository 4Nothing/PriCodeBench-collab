static inline uint32_t ipv6_hdr_get_fl(const ipv6_hdr_t *hdr)
{
    return byteorder_ntohl(hdr->v_tc_fl) & 0x000fffff;
}