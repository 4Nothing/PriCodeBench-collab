static inline bool ipv6_ext_frag_more(const ipv6_ext_frag_t *frag)
{
    return (byteorder_ntohs(frag->offset_flags) & IPV6_EXT_FRAG_M);
}
