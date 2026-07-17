void turo_container_close(turo_t *ctx, int exit_status)
{
    (void)exit_status;
    assert(ctx->idx == 1);
    assert(ctx->states[ctx->idx] == TURO_STATE_READY);
    assert(ctx->states[0] == TURO_STATE_CONTAINER);
    ctx->idx--;
}