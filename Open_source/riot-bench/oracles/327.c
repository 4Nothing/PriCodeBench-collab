void turo_simple_array_u8(turo_t *ctx, uint8_t *vals, size_t size)
{
    turo_container_open(ctx);
    turo_array_u8(ctx, vals, size);
    turo_container_close(ctx, 0);
}