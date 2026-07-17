void nanocoap_cache_key_blockreq_options_generate(const coap_pkt_t *req, void *cache_key)
{
    sha256_context_t ctx;
    sha256_init(&ctx);
    _cache_key_digest_opts(req, &ctx, true, false);
    sha256_final(&ctx, cache_key);
}

