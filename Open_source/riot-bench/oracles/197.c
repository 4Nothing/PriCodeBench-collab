int ieee802154_get_src(const uint8_t *mhr, uint8_t *src, le_uint16_t *src_pan)
{
    int offset = 3; /* FCF: 0-1, Seq: 2 */
    uint8_t tmp;

    assert((src != NULL) && (src_pan != NULL));
    tmp = mhr[1] & IEEE802154_FCF_DST_ADDR_MASK;
    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        if (mhr[0] & IEEE802154_FCF_PAN_COMP) {
            src_pan->u8[0] = mhr[offset];
            src_pan->u8[1] = mhr[offset + 1];
        }
        offset += 4;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        if (mhr[0] & IEEE802154_FCF_PAN_COMP) {
            src_pan->u8[0] = mhr[offset];
            src_pan->u8[1] = mhr[offset + 1];
        }
        offset += 10;
    }
    else if (tmp != IEEE802154_FCF_DST_ADDR_VOID) {
        return -EINVAL;
    }
    else if (mhr[0] & IEEE802154_FCF_PAN_COMP) {
        /* PAN compression, but no destination address => illegal state */
        return -EINVAL;
    }

    tmp = mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK;
    if (tmp != IEEE802154_FCF_SRC_ADDR_VOID) {
        if (!(mhr[0] & IEEE802154_FCF_PAN_COMP) &&
            (tmp != IEEE802154_FCF_SRC_ADDR_RESV)) {
            src_pan->u8[0] = mhr[offset++];
            src_pan->u8[1] = mhr[offset++];
        }
    }
    if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
        /* read src PAN and address in little endian */
        src[1] = mhr[offset++];
        src[0] = mhr[offset++];
        return 2;
    }
    else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
        /* read src PAN and address in little endian */
        for (int i = 7; i >= 0; i--) {
            src[i] = mhr[offset++];
        }
        return 8;
    }
    else if (tmp != IEEE802154_FCF_SRC_ADDR_VOID) {
        return -EINVAL;
    }

    return 0;
}
