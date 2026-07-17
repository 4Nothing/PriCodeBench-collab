static inline void ipv6_addr_set_loopback(ipv6_addr_t *addr)
{
    memset(addr, 0, sizeof(ipv6_addr_t));
    addr->u8[15] = 1;
}