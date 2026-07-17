uint16_t fletcher16(const uint8_t *data, size_t bytes)
{
    fletcher16_ctx_t ctx;
    fletcher16_init(&ctx);
    fletcher16_update(&ctx, data, bytes);
    return fletcher16_finish(&ctx);
}
