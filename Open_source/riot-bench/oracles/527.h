static inline bool ipv6_addr_is_solicited_node(const ipv6_addr_t *addr)
{
    return (memcmp(addr, &ipv6_addr_solicited_node_prefix,
                   sizeof(ipv6_addr_t) - 3) == 0);
}