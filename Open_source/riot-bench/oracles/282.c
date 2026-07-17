void sha3_512_final(keccak_state_t *ctx, void *digest)
{
    Keccak_final(ctx, digest, SHA3_512_DIGEST_LENGTH);
}
