static inline uint16_t sixlowpan_sfr_rfrag_get_offset(const sixlowpan_sfr_rfrag_t *hdr)
{
    return byteorder_ntohs(hdr->offset);
}
