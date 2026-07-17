static inline bool ipv6_addr_is_link_local(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_link_local_prefix, sizeof(addr->u64[0])) == 0) ||
           (ipv6_addr_is_multicast(addr) &&
            (addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
}