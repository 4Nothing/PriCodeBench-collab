static inline void sixlowpan_sfr_rfrag_set_ack_req(sixlowpan_sfr_rfrag_t *hdr)
{
    hdr->ar_seq_fs.u8[0] |= SIXLOWPAN_SFR_ACK_REQ;
}
