static inline bool sixlowpan_sfr_rfrag_is(const sixlowpan_sfr_t *hdr)
{
    return ((hdr->disp_ecn & SIXLOWPAN_SFR_DISP_MASK) == SIXLOWPAN_SFR_RFRAG_DISP);
}
