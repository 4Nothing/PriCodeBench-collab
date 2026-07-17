static inline void ipv6_addr_set_link_local_prefix(ipv6_addr_t *addr)
{
    memcpy(addr, &ipv6_addr_link_local_prefix, sizeof(addr->u64[0]));
}