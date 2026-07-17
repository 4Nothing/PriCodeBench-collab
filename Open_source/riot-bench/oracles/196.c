size_t ieee802154_get_frame_hdr_len(const uint8_t *mhr)
{
    /* TODO: include security header implications */
    uint8_t tmp, has_dst = 0;
    size_t len = 3; /* 2 byte FCF, 1 byte sequence number */

    tmp = (mhr[0] & IEEE802154_FCF_TYPE_MASK);
    if (tmp == IEEE802154_FCF_TYPE_ACK) {
        /* ACK contains no other fields */
        return len;
    } else if (tmp != IEEE802154_FCF_TYPE_BEACON) {
        /* Beacon contains no dst address */
        tmp = (mhr[1] & IEEE802154_FCF_DST_ADDR_MASK);
        if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
            len += 4;   /* 2 byte dst PAN + 2 byte dst short address */
            has_dst = 1;
        }
        else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
            len += 10;  /* 2 byte dst PAN + 8 byte dst long address */
            has_dst = 1;
        }
        else if (tmp != IEEE802154_FCF_DST_ADDR_VOID) {
            return 0;
        }
        else if (mhr[0] & IEEE802154_FCF_PAN_COMP) {
            /* PAN compression, but no destination address => illegal state */
            return 0;
        }
    } else if (mhr[0] & IEEE802154_FCF_PAN_COMP) {
        /* Beacon can't use PAN compression */
        return 0;
    }
    tmp = (mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK);
    if (tmp == IEEE802154_FCF_SRC_ADDR_VOID) {
        /* One of dst or src address must be present */
        return has_dst ? len : 0;
    }
    else {
        if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
            len += 2;
        }
        if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
            return len + 2;
        }
        else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
            return len + 8;
        }
    }
    return 0;
}
