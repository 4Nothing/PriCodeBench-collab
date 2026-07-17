void sha1_final_hmac(sha1_context *ctx, void *digest)
{
    uint8_t i;

    /* Complete inner hash */
    sha1_final(ctx, ctx->inner_hash);
    /* Calculate outer hash */
    sha1_init(ctx);
    for (i = 0; i < SHA1_BLOCK_LENGTH; i++) {
        sha1_update_byte(ctx, ctx->key_buffer[i] ^ HMAC_OPAD);
    }
    for (i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        sha1_update_byte(ctx, ctx->inner_hash[i]);
    }

    sha1_final(ctx, digest);
}
