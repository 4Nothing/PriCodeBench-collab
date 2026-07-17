static inline bool ipv6_addr_is_loopback(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_loopback, sizeof(ipv6_addr_t)) == 0);
}