void hmac_sha256_init(hmac_context_t *ctx, const void *key, size_t key_length)
{
    unsigned char k[SHA256_INTERNAL_BLOCK_SIZE];

    memset((void *)k, 0x00, SHA256_INTERNAL_BLOCK_SIZE);

    if (key_length > SHA256_INTERNAL_BLOCK_SIZE) {
        sha256(key, key_length, k);
    }
    else {
        memcpy((void *)k, key, key_length);
    }

    /*
     * create the inner and outer keypads
     * rising hamming distance enforcing i_* and o_* are distinct
     * in at least one bit
     */
    unsigned char o_key_pad[SHA256_INTERNAL_BLOCK_SIZE];
    unsigned char i_key_pad[SHA256_INTERNAL_BLOCK_SIZE];

    for (size_t i = 0; i < SHA256_INTERNAL_BLOCK_SIZE; ++i) {
        o_key_pad[i] = 0x5c ^ k[i];
        i_key_pad[i] = 0x36 ^ k[i];
    }

    /*
     * Initiate calculation of the inner hash
     * tmp = hash(i_key_pad CONCAT message)
     */
    sha256_init(&ctx->c_in);
    sha2xx_update(&ctx->c_in, i_key_pad, SHA256_INTERNAL_BLOCK_SIZE);

    /*
     * Initiate calculation of the outer hash
     * result = hash(o_key_pad CONCAT tmp)
     */
    sha256_init(&ctx->c_out);
    sha2xx_update(&ctx->c_out, o_key_pad, SHA256_INTERNAL_BLOCK_SIZE);

}
