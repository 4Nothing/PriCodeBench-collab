static inline bool sixlowpan_sfr_ack_is(const sixlowpan_sfr_t *hdr)
{
    return ((hdr->disp_ecn & SIXLOWPAN_SFR_DISP_MASK) == SIXLOWPAN_SFR_ACK_DISP);
}
