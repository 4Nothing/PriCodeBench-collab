static inline size_t coap_opt_put_location_path(uint8_t *buf,
                                                uint16_t lastonum,
                                                const char *location)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_LOCATION_PATH,
                               location, '/');
}
