static inline void ipv6_addr_set_aiid(ipv6_addr_t *addr, uint8_t *iid)
{
    memcpy(&addr->u64[1], iid, sizeof(addr->u64[1]));
}