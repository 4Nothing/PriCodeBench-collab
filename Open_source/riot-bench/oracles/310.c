void turo_array_close(turo_t *ctx)
{
    assert(ctx->idx > 0);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY);
    assert(ctx->states[--ctx->idx] == TURO_STATE_ARRAY_OPENED);
    ctx->states[ctx->idx] = TURO_STATE_READY;
}