static inline uint8_t sixlowpan_sfr_rfrag_get_seq(const sixlowpan_sfr_rfrag_t *hdr)
{
    return ((hdr->ar_seq_fs.u8[0] & SIXLOWPAN_SFR_SEQ_MASK) >>
            SIXLOWPAN_SFR_SEQ_POS);
}
