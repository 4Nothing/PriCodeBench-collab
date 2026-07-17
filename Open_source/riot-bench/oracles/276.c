void sha3_256_final(keccak_state_t *ctx, void *digest)
{
    Keccak_final(ctx, digest, SHA3_256_DIGEST_LENGTH);
}
