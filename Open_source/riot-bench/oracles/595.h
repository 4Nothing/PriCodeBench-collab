static inline void sixlowpan_sfr_rfrag_set_seq(sixlowpan_sfr_rfrag_t *hdr,
                                               uint8_t seq)
{
    assert(seq <= SIXLOWPAN_SFR_SEQ_MAX);
    hdr->ar_seq_fs.u8[0] &= ~SIXLOWPAN_SFR_SEQ_MASK;
    hdr->ar_seq_fs.u8[0] |= (seq << SIXLOWPAN_SFR_SEQ_POS);
}
