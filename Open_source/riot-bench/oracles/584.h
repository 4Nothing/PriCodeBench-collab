static inline bool sixlowpan_sfr_ecn(const sixlowpan_sfr_t *hdr)
{
    return (hdr->disp_ecn & SIXLOWPAN_SFR_ECN);
}
