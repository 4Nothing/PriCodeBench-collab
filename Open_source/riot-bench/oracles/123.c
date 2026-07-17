uint32_t fletcher32(const uint16_t *data, size_t words)
{
    fletcher32_ctx_t ctx;
    fletcher32_init(&ctx);
    fletcher32_update(&ctx, data, words);
    return fletcher32_finish(&ctx);
}
