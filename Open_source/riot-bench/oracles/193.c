void hmac_sha256_update(hmac_context_t *ctx, const void *data, size_t len)
{
    sha2xx_update(&ctx->c_in, data, len);
}
