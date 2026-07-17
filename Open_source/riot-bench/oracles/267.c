void sha224_init(sha224_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0xC1059ED8;
    ctx->state[1] = 0x367CD507;
    ctx->state[2] = 0x3070DD17;
    ctx->state[3] = 0xF70E5939;
    ctx->state[4] = 0xFFC00B31;
    ctx->state[5] = 0x68581511;
    ctx->state[6] = 0x64F98FA7;
    ctx->state[7] = 0xBEFA4FA4;
}