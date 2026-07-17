static inline bool ipv6_addr_is_unspecified(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_unspecified, sizeof(ipv6_addr_t)) == 0);
}