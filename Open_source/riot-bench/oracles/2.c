void aes128_cmac_final(aes128_cmac_context_t *ctx, void *digest)
{
    /* Generate subkeys */
    uint8_t K[AES128_CMAC_BLOCK_SIZE];
    uint8_t L[AES128_CMAC_BLOCK_SIZE];

    memset(K, 0, AES128_CMAC_BLOCK_SIZE);
    cipher_encrypt(&ctx->aes128_ctx, K, L);

    if (L[0] & 0x80) {
        _leftshift(L, K);
        K[15] ^= 0x87;
    }
    else {
        _leftshift(L, K);
    }

    if (ctx->M_n != 16) {
        /* Generate K2 */
        if (K[0] & 0x80) {
            _leftshift(K, K);
            K[15] ^= 0x87;
        }
        else {
            _leftshift(K, K);
        }
        /* Padding */
        memset(ctx->M_last + ctx->M_n, 0, AES128_CMAC_BLOCK_SIZE - ctx->M_n);
        ctx->M_last[ctx->M_n] = 0x80;
    }
    _xor128(K, ctx->M_last);
    _xor128(ctx->M_last, ctx->X);
    cipher_encrypt(&ctx->aes128_ctx, ctx->X, L);
    memcpy(digest, L, AES128_CMAC_BLOCK_SIZE);
}
