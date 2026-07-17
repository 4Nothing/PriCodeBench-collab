void turo_simple_exit_status(turo_t *ctx, int exit_status)
{
    turo_container_open(ctx);
    turo_container_close(ctx, exit_status);
}