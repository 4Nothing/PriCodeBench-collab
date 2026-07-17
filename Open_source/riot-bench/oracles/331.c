void turo_simple_s32(turo_t *ctx, int32_t val)
{
    turo_container_open(ctx);
    turo_s32(ctx, val);
    turo_container_close(ctx, 0);
}