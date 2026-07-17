void sha3_384_final(keccak_state_t *ctx, void *digest)
{
    Keccak_final(ctx, digest, SHA3_384_DIGEST_LENGTH);
}
