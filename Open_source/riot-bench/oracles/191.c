void hmac_sha256_final(hmac_context_t *ctx, void *digest)
{
    unsigned char tmp[SHA256_DIGEST_LENGTH];

    sha256_final(&ctx->c_in, tmp);
    sha2xx_update(&ctx->c_out, tmp, SHA256_DIGEST_LENGTH);
    sha256_final(&ctx->c_out, digest);
}
