static inline void sixlowpan_sfr_clear_ecn(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn &= ~SIXLOWPAN_SFR_ECN;
}
