static inline void sha224_final(sha224_context_t *ctx, void *digest)
{
    sha2xx_final(ctx, digest, SHA224_DIGEST_LENGTH);
}
