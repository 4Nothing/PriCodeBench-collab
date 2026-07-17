void fletcher32_init(fletcher32_ctx_t *ctx)
{
    ctx->sum1 = 0xffff;
    ctx->sum2 = 0xffff;
}
