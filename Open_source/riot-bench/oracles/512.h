static inline void gnrc_sixlowpan_frag_vrb_rm(gnrc_sixlowpan_frag_vrb_t *vrb)
{
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_RB)) {
        gnrc_sixlowpan_frag_rb_base_rm(&vrb->super);
    }
    vrb->super.src_len = 0;
}
