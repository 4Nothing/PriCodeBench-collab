void turo_array_s32(turo_t *ctx, int32_t *vals, size_t size)
{
    turo_array_open(ctx);
    while (size-- > 0) {
        turo_s32(ctx, *vals);
        vals++;
    }
    turo_array_close(ctx);
}