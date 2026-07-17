static inline void ipv6_addr_set_all_routers_multicast(ipv6_addr_t *addr, unsigned int scope)
{
    memcpy(addr, &ipv6_addr_all_routers_if_local, sizeof(ipv6_addr_t));
    addr->u8[1] = (uint8_t)scope;
}