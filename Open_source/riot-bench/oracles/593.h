static inline void sixlowpan_sfr_rfrag_set_frag_size(sixlowpan_sfr_rfrag_t *hdr,
                                                     uint16_t frag_size)
{
    assert(frag_size <= SIXLOWPAN_SFR_FRAG_SIZE_MAX);
    hdr->ar_seq_fs.u16 &= ~htons(SIXLOWPAN_SFR_FRAG_SIZE_MASK);
    hdr->ar_seq_fs.u16 |= htons(frag_size);
}
