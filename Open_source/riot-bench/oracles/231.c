nanocoap_cache_entry_t *nanocoap_cache_add_by_req(const coap_pkt_t *req,
                                                  const coap_pkt_t *resp,
                                                  size_t resp_len)
{
    uint8_t cache_key[SHA256_DIGEST_LENGTH];

    /* generate cache key */
    nanocoap_cache_key_generate(req, cache_key);

    return nanocoap_cache_add_by_key(cache_key,
                                     coap_get_code_raw((coap_pkt_t *)req),
                                     resp,
                                     resp_len);
}

