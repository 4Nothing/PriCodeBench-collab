static inline void ipv6_ext_frag_set_more(ipv6_ext_frag_t *frag)
{
    frag->offset_flags.u8[1] |= IPV6_EXT_FRAG_M;
}
