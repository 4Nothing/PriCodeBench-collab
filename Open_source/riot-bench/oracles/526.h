static inline bool ipv6_addr_is_site_local(const ipv6_addr_t *addr)
{
    return (((byteorder_ntohs(addr->u16[0]) & 0xffc0) ==
             IPV6_ADDR_SITE_LOCAL_PREFIX) ||
            (ipv6_addr_is_multicast(addr) &&
             (addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_SITE_LOCAL));
}