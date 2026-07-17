void sha3_384_init(keccak_state_t *ctx)
{
    Keccak_init(ctx, 832, 768, 0x06);
}
