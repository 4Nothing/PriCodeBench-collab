void turo_simple_dict_string(turo_t *ctx, const char *key, const char *val)
{
    turo_container_open(ctx);
    turo_dict_open(ctx);
    turo_dict_key(ctx, key);
    turo_string(ctx, val);
    turo_dict_close(ctx);
    turo_container_close(ctx, 0);
}