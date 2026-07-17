static inline void sha512_final(sha512_context_t *ctx, void *digest)
{
    sha512_common_final(ctx, digest, SHA512_DIGEST_LENGTH);
}
