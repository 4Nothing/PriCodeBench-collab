static inline uint16_t ipv6_hdr_inet_csum(uint16_t sum, ipv6_hdr_t *hdr,
                                          uint8_t prot_num, uint16_t len)
{
    if (((uint32_t)sum + len + prot_num) > 0xffff) {
        /* increment by one for overflow to keep it as 1's complement sum */
        sum++;
    }

    return inet_csum(sum + len + prot_num, hdr->src.u8,
                     (2 * sizeof(ipv6_addr_t)));
}