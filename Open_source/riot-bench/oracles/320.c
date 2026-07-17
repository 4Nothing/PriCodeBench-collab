void turo_dict_s32(turo_t *ctx, const char *key, int32_t val)
{
    turo_dict_open(ctx);
    turo_dict_key(ctx, key);
    turo_s32(ctx, val);
    turo_dict_close(ctx);
}