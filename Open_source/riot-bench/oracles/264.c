void sha1_init_hmac(sha1_context *ctx, const void *key, size_t key_length)
{
    uint8_t i;
    const uint8_t *k = key;

    memset(ctx->key_buffer, 0, SHA1_BLOCK_LENGTH);
    if (key_length > SHA1_BLOCK_LENGTH) {
        /* Hash long keys */
        sha1_init(ctx);
        while (key_length--) {
            sha1_update_byte(ctx, *k++);
        }
        sha1_final(ctx, ctx->key_buffer);
    }
    else {
        /* Block length keys are used as is */
        memcpy(ctx->key_buffer, key, key_length);
    }
    /* Start inner hash */
    sha1_init(ctx);
    for (i = 0; i < SHA1_BLOCK_LENGTH; i++) {
        sha1_update_byte(ctx, ctx->key_buffer[i] ^ HMAC_IPAD);
    }
}
