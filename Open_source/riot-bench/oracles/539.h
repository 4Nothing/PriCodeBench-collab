static inline char *ipv6_addr_split_iface(char *addr_str)
{
    return ipv6_addr_split_str(addr_str, '%');
}