void turo_simple_array_s32(turo_t *ctx, int32_t *vals, size_t size)
{
    turo_container_open(ctx);
    turo_array_s32(ctx, vals, size);
    turo_container_close(ctx, 0);
}