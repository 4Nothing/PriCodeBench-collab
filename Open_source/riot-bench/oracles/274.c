void sha384_init(sha384_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0xcbbb9d5dc1059ed8;
    ctx->state[1] = 0x629a292a367cd507;
    ctx->state[2] = 0x9159015a3070dd17;
    ctx->state[3] = 0x152fecd8f70e5939;
    ctx->state[4] = 0x67332667ffc00b31;
    ctx->state[5] = 0x8eb44a8768581511;
    ctx->state[6] = 0xdb0c2e0d64f98fa7;
    ctx->state[7] = 0x47b5481dbefa4fa4;
}
