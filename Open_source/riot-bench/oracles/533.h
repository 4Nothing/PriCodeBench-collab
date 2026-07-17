static inline void ipv6_addr_set_iid(ipv6_addr_t *addr, uint64_t iid)
{
    addr->u64[1] = byteorder_htonll(iid);
}