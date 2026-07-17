static inline void sha384_final(sha384_context_t *ctx, void *digest)
{
    sha512_common_final(ctx, digest, SHA384_DIGEST_LENGTH);
}
