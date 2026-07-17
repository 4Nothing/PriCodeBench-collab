void gnrc_sixlowpan_frag_fb_reset(void)
{
    memset(_fbs, 0, sizeof(_fbs));
    _current_tag = 0;
}
