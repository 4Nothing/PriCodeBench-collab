static inline void sixlowpan_sfr_ack_set_disp(sixlowpan_sfr_t *hdr)
{
    hdr->disp_ecn &= ~SIXLOWPAN_SFR_DISP_MASK;
    hdr->disp_ecn |= SIXLOWPAN_SFR_ACK_DISP;
}
