static inline void ipv6_addr_set_unspecified(ipv6_addr_t *addr)
{
    memset(addr, 0, sizeof(ipv6_addr_t));
}