inline eui64_t *ieee802154_get_iid(eui64_t *eui64, const uint8_t *addr,
                                          size_t addr_len)
{
    int i = 0;

    eui64->uint8[0] = eui64->uint8[1] = 0;

    switch (addr_len) {
        case 8:
            eui64->uint8[0] = addr[i++] ^ 0x02;
            eui64->uint8[1] = addr[i++];
            eui64->uint8[2] = addr[i++];
            eui64->uint8[3] = addr[i++];
            eui64->uint8[4] = addr[i++];
            eui64->uint8[5] = addr[i++];
            eui64->uint8[6] = addr[i++];
            eui64->uint8[7] = addr[i++];
            break;

        case 4:
            eui64->uint8[0] = addr[i++] ^ 0x02;
            eui64->uint8[1] = addr[i++];

            /* Falls through. */
        case 2:
            eui64->uint8[2] = 0;
            eui64->uint8[3] = 0xff;
            eui64->uint8[4] = 0xfe;
            eui64->uint8[5] = 0;
            eui64->uint8[6] = addr[i++];
            eui64->uint8[7] = addr[i++];
            break;

        default:
            return NULL;
    }

    return eui64;
}

/**
 * @