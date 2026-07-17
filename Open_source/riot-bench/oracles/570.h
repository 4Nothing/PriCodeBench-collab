static inline void sha256_final(sha256_context_t *ctx, void *digest)
{
    sha2xx_final(ctx, digest, SHA256_DIGEST_LENGTH);
}
