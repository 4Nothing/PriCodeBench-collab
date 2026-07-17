void turo_dict_key(turo_t *ctx, const char *key)
{
    (void)key;
    assert(ctx->idx > 0);
    assert(ctx->states[ctx->idx - 1] == TURO_STATE_DICT_OPENED);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY ||
           ctx->states[ctx->idx] == TURO_STATE_DICT_OPENED);
    ctx->states[ctx->idx] = TURO_STATE_READY;
}