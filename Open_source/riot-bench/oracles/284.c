void sha3_update(keccak_state_t *ctx, const void *data, size_t len)
{
    Keccak_update(ctx, data, len);
}
