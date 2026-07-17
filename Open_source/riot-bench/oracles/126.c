void fletcher32_update(fletcher32_ctx_t *ctx, const void *data, size_t words)
{
    const uint16_t *u16_data = (const uint16_t*)data;
    while (words) {
        unsigned tlen = words > 359 ? 359 : words;
        words -= tlen;
        do {
            ctx->sum1 += unaligned_get_u16(u16_data++);
            ctx->sum2 += ctx->sum1;
        } while (--tlen);
        _reduce(ctx);
    }
}
