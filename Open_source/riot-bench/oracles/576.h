static inline void sha512_256_final(sha512_256_context_t *ctx, void *digest)
{
    sha512_common_final(ctx, digest, SHA512_256_DIGEST_LENGTH);
}
