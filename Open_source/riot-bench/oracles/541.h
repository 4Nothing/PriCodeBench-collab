static inline unsigned ipv6_ext_frag_get_offset(const ipv6_ext_frag_t *frag)
{
    /* The offset is left-shifted by 3 bytes in the header * (equivalent to
     * multiplication with 8), and the offset is in units 8 bytes
     * so no shifting or multiplying necessary to get offset in bytes */
    return (byteorder_ntohs(frag->offset_flags) & IPV6_EXT_FRAG_OFFSET_MASK);
}
