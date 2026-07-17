void sha512_256_init(sha512_256_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0x22312194FC2BF72C;
    ctx->state[1] = 0x9F555FA3C84C64C2;
    ctx->state[2] = 0x2393B86B6F53B151;
    ctx->state[3] = 0x963877195940EABD;
    ctx->state[4] = 0x96283EE2A88EFFE3;
    ctx->state[5] = 0xBE5E1E2553863992;
    ctx->state[6] = 0x2B0199FC2C85B8AA;
    ctx->state[7] = 0x0EB72DDC81C52CA2;
}
