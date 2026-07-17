int aes128_cmac_init(aes128_cmac_context_t *ctx,
                     const uint8_t *key, uint8_t key_size)
{
    if (key_size != AES128_CMAC_BLOCK_SIZE) {
        return CIPHER_ERR_INVALID_KEY_SIZE;
    }

    memset(ctx, 0, sizeof(aes128_cmac_context_t));
    return cipher_init(&(ctx->aes128_ctx), CIPHER_AES, key, key_size);
}

