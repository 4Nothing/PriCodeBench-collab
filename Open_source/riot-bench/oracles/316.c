void turo_container_open(turo_t *ctx)
{
    assert(ctx->idx == 0);
    assert(ctx->idx < CONFIG_TURO_MAX_NESTING_LEVELS);
    ctx->states[ctx->idx++] = TURO_STATE_CONTAINER;
    ctx->states[ctx->idx] = TURO_STATE_READY;
}