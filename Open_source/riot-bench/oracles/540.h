static inline int ipv6_addr_split_prefix(char *addr_str)
{
    return ipv6_addr_split_int(addr_str, '/', 128);
}