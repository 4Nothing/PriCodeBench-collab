ipv4_addr_t *ipv4_addr_from_str(ipv4_addr_t *result, const char *addr)
{
    if ((result == NULL) || (addr == NULL)) {
        return NULL;
    }

    return ipv4_addr_from_buf(result, addr, strlen(addr));
}