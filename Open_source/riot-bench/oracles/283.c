void sha3_512_init(keccak_state_t *ctx)
{
    Keccak_init(ctx, 576, 1024, 0x06);
}
