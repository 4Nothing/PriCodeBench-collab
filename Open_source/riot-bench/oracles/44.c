ssize_t coap_opt_add_chars(coap_pkt_t *pkt, uint16_t optnum, const char *chars,
                           size_t chars_len, char separator)
{
    /* chars_len denotes the length of the chars buffer and is
     * gradually decremented below while iterating over the buffer */
    if (!chars_len) {
        return 0;
    }

    char *uripos = (char *)chars;
    char *endpos = ((char *)chars + chars_len);
    size_t write_len = 0;

    while (chars_len) {
        size_t part_len;
        if (*uripos == separator) {
            uripos++;
        }
        uint8_t *part_start = (uint8_t *)uripos;

        while (chars_len) {
            /* must decrement separately from while loop test to ensure
             * the value remains non-negative */
            chars_len--;
            if ((*uripos == separator) || (uripos == endpos)) {
                break;
            }
            uripos++;
        }

        part_len = (uint8_t *)uripos - part_start;

        /* Creates empty option if part for Uri-Path or Uri-Location contains
         * only a trailing slash, except for root path ("/"). */
        if (part_len || ((separator == '/') && write_len)) {
            ssize_t optlen = _add_opt_pkt(pkt, optnum, part_start, part_len);
            if (optlen < 0) {
                return optlen;
            }
            write_len += optlen;
        }
    }

    return write_len;
}
