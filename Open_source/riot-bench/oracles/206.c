ipv6_addr_t *ipv6_addr_from_str(ipv6_addr_t *result, const char *addr)
{
    if ((result == NULL) || (addr == NULL)) {
        return NULL;
    }

    return ipv6_addr_from_buf(result, addr, strlen(addr));
}