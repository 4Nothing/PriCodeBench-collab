size_t ieee802154_set_frame_hdr(uint8_t *buf, const uint8_t *src, size_t src_len,
                                const uint8_t *dst, size_t dst_len,
                                le_uint16_t src_pan, le_uint16_t dst_pan,
                                uint8_t flags, uint8_t seq)
{
    int pos = 3;    /* 0-1: FCS, 2: seq */
    uint8_t type = (flags & IEEE802154_FCF_TYPE_MASK);

    buf[0] = flags;
    buf[1] = IEEE802154_FCF_VERS_V1;

    if (((src_len != 0) && (src == NULL)) ||
        ((dst_len != 0) && (dst == NULL))) {
        return 0;
    }

    /* Frame type is not beacon or ACK, but both address modes are zero */
    if ((type != IEEE802154_FCF_TYPE_BEACON) && (type != IEEE802154_FCF_TYPE_ACK) &&
        (src_len == 0) && (dst_len == 0)) {
        return 0;
    }

    /* set sequence number */
    buf[2] = seq;

    if (dst_len != 0) {
        buf[pos++] = dst_pan.u8[0];
        buf[pos++] = dst_pan.u8[1];
    }

    /* fill in destination address */
    switch (dst_len) {
        case 0:
            buf[1] |= IEEE802154_FCF_DST_ADDR_VOID;
            break;
        case 2:
            if (memcmp(dst, ieee802154_addr_bcast,
                       sizeof(ieee802154_addr_bcast)) == 0) {
                /* do not request ACKs for broadcast address */
                buf[0] &= ~IEEE802154_FCF_ACK_REQ;
            }
            buf[1] |= IEEE802154_FCF_DST_ADDR_SHORT;
            buf[pos++] = dst[1];
            buf[pos++] = dst[0];
            break;
        case 8:
            buf[1] |= IEEE802154_FCF_DST_ADDR_LONG;
            for (int i = 7; i >= 0; i--) {
                buf[pos++] = dst[i];
            }
            break;
        default:
            return 0;
    }

    /* fill in source PAN ID (if applicable) */
    if (src_len != 0) {
        if ((dst_len != 0) && (src_pan.u16 == dst_pan.u16)) {
            buf[0] |= IEEE802154_FCF_PAN_COMP;
        }
        else {
            /* (little endian) */
            buf[pos++] = src_pan.u8[0];
            buf[pos++] = src_pan.u8[1];
        }
    }

    /* fill in source address */
    switch (src_len) {
        case 0:
            buf[1] |= IEEE802154_FCF_SRC_ADDR_VOID;
            break;
        case 2:
            buf[1] |= IEEE802154_FCF_SRC_ADDR_SHORT;
            buf[pos++] = src[1];
            buf[pos++] = src[0];
            break;
        case 8:
            buf[1] |= IEEE802154_FCF_SRC_ADDR_LONG;
            for (int i = 7; i >= 0; i--) {
                buf[pos++] = src[i];
            }
            break;
        default:
            return 0;
    }

    /* return actual header length */
    return pos;
}
