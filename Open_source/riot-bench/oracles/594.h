static inline void sixlowpan_sfr_rfrag_set_offset(sixlowpan_sfr_rfrag_t *hdr,
                                                  uint16_t offset)
{
    hdr->offset = byteorder_htons(offset);
}
