void fletcher16_init(fletcher16_ctx_t *ctx)
{
    ctx->sum1 = 0xff;
    ctx->sum2 = 0xff;
}
