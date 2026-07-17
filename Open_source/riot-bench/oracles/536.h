static inline void ipv6_addr_set_multicast(ipv6_addr_t *addr, unsigned int flags,
                                           unsigned int scope)
{
    addr->u8[0] = 0xff;
    addr->u8[1] = (((uint8_t)flags) << 4) | (((uint8_t) scope) & 0x0f);
}