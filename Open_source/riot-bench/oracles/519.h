static inline void ipv6_addr_init(ipv6_addr_t *out, uint64_t prefix, uint64_t iid)
{
    out->u64[0] = byteorder_htonll(prefix);
    out->u64[1] = byteorder_htonll(iid);
}