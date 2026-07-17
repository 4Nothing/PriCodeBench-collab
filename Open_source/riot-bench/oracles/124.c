uint32_t fletcher32_finish(fletcher32_ctx_t *ctx)
{
    /* Second reduction step to reduce sums to 8 bits */
    _reduce(ctx);
    return (ctx->sum2 << 16) | ctx->sum1;
}
