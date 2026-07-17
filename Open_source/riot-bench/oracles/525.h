static inline bool ipv6_addr_is_multicast(const ipv6_addr_t *addr)
{
    return (addr->u8[0] == 0xff);
}