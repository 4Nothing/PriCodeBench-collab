static inline void sha224_update(sha224_context_t *ctx, const void *data, size_t len)
{
    sha2xx_update(ctx, data, len);
}
