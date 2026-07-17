void md5(void *digest, const void *data, size_t len)
{
    md5_ctx_t ctx;

    md5_init(&ctx);
    md5_update(&ctx, data, len);
    md5_final(&ctx, digest);
}
