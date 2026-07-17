void sha3_256_init(keccak_state_t *ctx)
{
    Keccak_init(ctx, 1088, 512, 0x06);
}
