static inline int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                 unsigned code, const char *path)
{
    return gcoap_req_init_path_buffer(pdu, buf, len, code, path,
                                      (path) ? strlen(path) : 0U);
}
