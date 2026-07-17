void turo_array_open(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->idx < CONFIG_TURO_MAX_NESTING_LEVELS);
    ctx->states[ctx->idx++] = TURO_STATE_ARRAY_OPENED;
    ctx->states[ctx->idx] = TURO_STATE_READY;
}