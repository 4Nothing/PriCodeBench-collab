static inline void ipv6_ext_frag_set_offset(ipv6_ext_frag_t *frag,
                                            unsigned offset)
{
    /* The offset is left-shifted by 3 bytes in the header * (equivalent to
     * multiplication with 8), and the offset is a multiple of 8 bytes
     * so no shifting or division necessary to set offset in units of 8 bytes */
    frag->offset_flags = byteorder_htons(offset & IPV6_EXT_FRAG_OFFSET_MASK);
}
