static inline void sha256_update(sha256_context_t *ctx, const void *data, size_t len)
{
    sha2xx_update(ctx, data, len);
}
