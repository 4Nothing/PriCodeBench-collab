int ieee802154_get_dst(const uint8_t *mhr, uint8_t *dst, le_uint16_t *dst_pan)
{
    int offset = 3; /* FCF: 0-1, Seq: 2 */
    uint8_t tmp;

    assert((dst != NULL) && (dst_pan != NULL));
    tmp = mhr[1] & IEEE802154_FCF_DST_ADDR_MASK;
    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        /* read dst PAN and address in little endian */
        dst_pan->u8[0] = mhr[offset++];
        dst_pan->u8[1] = mhr[offset++];
        dst[1] = mhr[offset++];
        dst[0] = mhr[offset++];
        return 2;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        dst_pan->u8[0] = mhr[offset++];
        dst_pan->u8[1] = mhr[offset++];
        for (int i = 7; i >= 0; i--) {
            dst[i] = mhr[offset++];
        }
        return 8;
    }
    else if (tmp != IEEE802154_FCF_DST_ADDR_VOID) {
        return -EINVAL;
    }
    else if (mhr[0] & IEEE802154_FCF_PAN_COMP) {
        /* PAN compression, but no destination address => illegal state */
        return -EINVAL;
    }

    return 0;
}
