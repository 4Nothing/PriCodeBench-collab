void fletcher16_update(fletcher16_ctx_t *ctx, const uint8_t *data, size_t len)
{
    while (len) {
        size_t tlen = len > 20 ? 20 : len;
        len -= tlen;
        do {
            ctx->sum2 += ctx->sum1 += *data++;
        } while (--tlen);
        _reduce(ctx);
    }
}
