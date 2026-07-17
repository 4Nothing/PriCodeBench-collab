static inline void sixlowpan_sfr_rfrag_set_disp(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn &= ~SIXLOWPAN_SFR_DISP_MASK;
    hdr->disp_ecn |= SIXLOWPAN_SFR_RFRAG_DISP;
}
