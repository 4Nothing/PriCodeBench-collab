static inline bool ipv6_addr_is_unique_local_unicast(const ipv6_addr_t *addr)
{
    return ((addr->u8[0] == 0xfc) || (addr->u8[0] == 0xfd));
}