void gnrc_sixlowpan_ctx_reset(void)
{
    memset(_ctxs, 0, sizeof(_ctxs));
}