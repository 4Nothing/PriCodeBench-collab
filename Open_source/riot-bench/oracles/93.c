int dns_msg_parse_reply(const uint8_t *buf, size_t len, int family,
                        void *addr_out, uint32_t *ttl)
{
    const uint8_t *buflim = buf + len;
    const dns_hdr_t *hdr = (dns_hdr_t *)buf;
    const uint8_t *bufpos = buf + sizeof(*hdr);

    /* skip all queries that are part of the reply */
    for (unsigned n = 0; n < ntohs(hdr->qdcount); n++) {
        ssize_t tmp = _skip_hostname(buf, len, bufpos);
        if (tmp < 0) {
            return tmp;
        }
        bufpos += tmp;
        /* skip type and class of query */
        bufpos += (RR_TYPE_LENGTH + RR_CLASS_LENGTH);
    }

    for (unsigned n = 0; n < ntohs(hdr->ancount); n++) {
        ssize_t tmp = _skip_hostname(buf, len, bufpos);
        if (tmp < 0) {
            return tmp;
        }
        bufpos += tmp;
        if ((bufpos + RR_TYPE_LENGTH + RR_CLASS_LENGTH +
             RR_TTL_LENGTH + sizeof(uint16_t)) >= buflim) {
            DEBUG("dns_msg: record beyond buf limit");
            return -EBADMSG;
        }
        uint16_t _type = ntohs(_get_short(bufpos));
        bufpos += RR_TYPE_LENGTH;
        uint16_t class = ntohs(_get_short(bufpos));
        bufpos += RR_CLASS_LENGTH;
        if (ttl) {
            *ttl = byteorder_bebuftohl(bufpos);
        }
        bufpos += RR_TTL_LENGTH;

        unsigned rdlen = ntohs(_get_short(bufpos));
        bufpos += RR_RDLENGTH_LENGTH;
        if ((bufpos + rdlen) > buflim) {
            return -EBADMSG;
        }

        DEBUG("dns_msg: type: %u, class: %u, len: %u\n", _type, class, rdlen);

        /* skip unwanted answers */
        if ((class != DNS_CLASS_IN) ||
                ((_type == DNS_TYPE_A) && (family == AF_INET6)) ||
                ((_type == DNS_TYPE_AAAA) && (family == AF_INET)) ||
                ! ((_type == DNS_TYPE_A) || ((_type == DNS_TYPE_AAAA))
                    )) {
            if (rdlen > len) {
                /* buffer wraps around memory space */
                return -EBADMSG;
            }
            bufpos += rdlen;
            /* other out-of-bound is checked in `_skip_hostname()` at start of
             * loop */
            continue;
        }
        if (((rdlen != INADDRSZ)  && (family == AF_INET))  ||
            ((rdlen != IN6ADDRSZ) && (family == AF_INET6)) ||
            ((rdlen != IN6ADDRSZ) && (rdlen != INADDRSZ) &&
             (family == AF_UNSPEC))) {
            return -EBADMSG;
        }

        memcpy(addr_out, bufpos, rdlen);
        return rdlen;
    }

    return -EBADMSG;
}