static inline void gnrc_sixlowpan_ctx_remove(uint8_t id)
{
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_CTX)) {
        gnrc_sixlowpan_ctx_lookup_id(id)->prefix_len = 0;
    }
}