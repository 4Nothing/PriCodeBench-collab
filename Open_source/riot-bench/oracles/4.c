void aes128_cmac_update(aes128_cmac_context_t *ctx,
                        const void *data, size_t len)
{
    uint8_t d[16];

    while (len) {
        uint8_t c;
        if (ctx->M_n == 16) {
            ctx->M_n = 0;
            _xor128(ctx->M_last, ctx->X);
            cipher_encrypt(&ctx->aes128_ctx, ctx->X, d);
            memcpy(ctx->X, d, AES128_CMAC_BLOCK_SIZE);
        }
        c = MIN(AES128_CMAC_BLOCK_SIZE - ctx->M_n, len);
        memcpy(ctx->M_last + ctx->M_n, data, c);
        ctx->M_n += c;
        len -= c;
        data = (void *) (((uint8_t *) data) + c);

        if (ctx->M_n < AES128_CMAC_BLOCK_SIZE) {
            break;
        }
    }
}

