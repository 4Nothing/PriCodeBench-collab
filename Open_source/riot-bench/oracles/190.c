void hmac_sha256(const void *key, size_t key_length,
                 const void *data, size_t len, void *digest)
{

    hmac_context_t ctx;

    hmac_sha256_init(&ctx, key, key_length);
    hmac_sha256_update(&ctx,data, len);
    hmac_sha256_final(&ctx, digest);
}
