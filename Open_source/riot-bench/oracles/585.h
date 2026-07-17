static inline bool sixlowpan_sfr_rfrag_ack_req(sixlowpan_sfr_rfrag_t *hdr)
{
    return (hdr->ar_seq_fs.u8[0] & SIXLOWPAN_SFR_ACK_REQ);
}
