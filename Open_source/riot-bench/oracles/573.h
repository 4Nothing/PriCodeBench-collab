static inline void sha384_update(sha384_context_t *ctx, const void *data, size_t len)
{
    sha512_common_update(ctx, data, len);
}
