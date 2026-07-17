void turo_array_u8(turo_t *ctx, uint8_t *vals, size_t size)
{
    turo_array_open(ctx);
    while (size > 0) {
        turo_u32(ctx, (uint32_t)*vals);
        vals++;
        size--;
    }
    turo_array_close(ctx);
}