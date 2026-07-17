static inline ssize_t gcoap_request(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                    unsigned code, char *path)
{
    if (gcoap_req_init(pdu, buf, len, code, path) == 0) {
        if (IS_USED(MODULE_NANOCOAP_CACHE)) {
            /* remove ETag option slack added for cache validation */
            coap_opt_remove(pdu, COAP_OPT_ETAG);
        }
        return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
    }
    return -1;
}
