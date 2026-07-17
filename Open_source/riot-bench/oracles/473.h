static inline ssize_t coap_opt_add_string(coap_pkt_t *pkt, uint16_t optnum,
                                          const char *string, char separator)
{
    return coap_opt_add_chars(pkt, optnum, string, strlen(string), separator);
}
