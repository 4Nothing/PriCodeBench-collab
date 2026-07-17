ipv6_addr_t *ipv6_addr_from_buf(ipv6_addr_t *result, const char *addr,
                                size_t addr_len)
{
    uint8_t *colonp = 0;
    const char *start = addr;
#ifdef MODULE_IPV4_ADDR
    const char *curtok = addr;
#endif
    uint32_t val = 0;
    uint8_t saw_xdigit = 0;
    uint8_t i = 0;

    if ((result == NULL) || (addr == NULL) || (addr_len == 0) ||
        (addr_len > IPV6_ADDR_MAX_STR_LEN)) {
        return NULL;
    }

    ipv6_addr_set_unspecified(result);

    /* Leading :: requires some special handling. */
    if (*addr == ':') {
        if (*++addr != ':') {
            return NULL;
        }
    }

    while ((size_t)(addr - start) < addr_len) {
        char ch = *addr++;
        const char *pch;
        const char *xdigits;

        if ((pch = strchr((xdigits = HEX_L), ch)) == NULL) {
            pch = strchr((xdigits = HEX_U), ch);
        }

        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);

            if (val > 0xffff) {
                return NULL;
            }

            saw_xdigit = 1;
            continue;
        }

        if (ch == ':') {
#ifdef MODULE_IPV4_ADDR
            curtok = addr;
#endif

            if (!saw_xdigit) {
                if (colonp != NULL) {
                    return NULL;
                }

                colonp = &(result->u8[i]);
                continue;
            }

            if ((i + sizeof(uint16_t)) > sizeof(ipv6_addr_t)) {
                return NULL;
            }

            result->u8[i++] = (uint8_t)(val >> 8) & 0xff;
            result->u8[i++] = (uint8_t) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }

#ifdef MODULE_IPV4_ADDR
        if (ch == '.' && ((i + sizeof(ipv4_addr_t)) <= sizeof(ipv6_addr_t)) &&
            ipv4_addr_from_buf((ipv4_addr_t *)(&(result->u8[i])),
                               curtok, addr_len - (curtok - start)) != NULL) {
            i += sizeof(ipv4_addr_t);
            saw_xdigit = 0;
            break;  /* '\0' was seen by ipv4_addr_from_str(). */
        }
#endif

        return NULL;
    }

    if (saw_xdigit) {
        if (i + sizeof(uint16_t) > sizeof(ipv6_addr_t)) {
            return NULL;
        }

        result->u8[i++] = (uint8_t)(val >> 8) & 0xff;
        result->u8[i++] = (uint8_t) val & 0xff;
    }

    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int32_t n = &(result->u8[i++]) - colonp;

        for (int32_t j = 1; j <= n; j++) {
            result->u8[sizeof(ipv6_addr_t) - j] = colonp[n - j];
            colonp[n - j] = 0;
        }

        i = sizeof(ipv6_addr_t);
    }

    if (i != sizeof(ipv6_addr_t)) {
        return NULL;
    }

    return result;
}