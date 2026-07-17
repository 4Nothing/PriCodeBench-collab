static inline void sha512_256_update(sha512_256_context_t *ctx, const void *data, size_t len)
{
    sha512_common_update(ctx, data, len);
}
