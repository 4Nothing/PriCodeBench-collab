static inline void sha512_224_final(sha512_224_context_t *ctx, void *digest)
{
    sha512_common_final(ctx, digest, SHA512_224_DIGEST_LENGTH);
}
