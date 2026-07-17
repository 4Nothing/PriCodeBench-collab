uint16_t fletcher16_finish(fletcher16_ctx_t *ctx)
{
    /* Second reduction step to reduce sums to 8 bits */
    _reduce(ctx);
    return (ctx->sum2 << 8) | ctx->sum1;
}
