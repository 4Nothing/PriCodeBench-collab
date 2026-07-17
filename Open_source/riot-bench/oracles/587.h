static inline uint16_t sixlowpan_sfr_rfrag_get_frag_size(const sixlowpan_sfr_rfrag_t *hdr)
{
    return (byteorder_ntohs(hdr->ar_seq_fs) & SIXLOWPAN_SFR_FRAG_SIZE_MASK);
}
