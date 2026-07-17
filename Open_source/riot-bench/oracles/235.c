void nanocoap_cache_key_generate(const coap_pkt_t *req, uint8_t *cache_key)
{
    sha256_context_t ctx;
    sha256_init(&ctx);

    _cache_key_digest_opts(req, &ctx, !(IS_USED(MODULE_GCOAP_FORWARD_PROXY)), true);
    switch (req->hdr->code) {
        case COAP_METHOD_FETCH:
            sha256_update(&ctx, req->payload, req->payload_len);
            break;
        default:
            break;
    }
    sha256_final(&ctx, cache_key);
}

