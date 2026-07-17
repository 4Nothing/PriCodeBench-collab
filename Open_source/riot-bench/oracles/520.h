static inline bool ipv6_addr_is_global(const ipv6_addr_t *addr)
{
    /* first check for multicast with global scope */
    if (ipv6_addr_is_multicast(addr)) {
        return ((addr->u8[1] & 0x0f) == IPV6_ADDR_MCAST_SCP_GLOBAL);
    }
    else {
        return !(ipv6_addr_is_link_local(addr) ||
                 ipv6_addr_is_unspecified(addr) ||
                 ipv6_addr_is_loopback(addr));
    }
}