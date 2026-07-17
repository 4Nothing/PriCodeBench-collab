static inline void ipv6_addr_set_solicited_nodes(ipv6_addr_t *out, const ipv6_addr_t *in)
{
    out->u64[0] = byteorder_htonll(0xff02000000000000ull);
    out->u32[2] = byteorder_htonl(1);
    out->u8[12] = 0xff;
    out->u8[13] = in->u8[13];
    out->u16[7] = in->u16[7];
}