int ieee802154_dst_filter(const uint8_t *mhr, uint16_t pan,
                          network_uint16_t short_addr, const eui64_t *ext_addr)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);

    /* filter PAN ID */
    if ((memcmp(pan_bcast, dst_pan.u8, 2) != 0) &&
        (memcmp(&pan, dst_pan.u8, 2) != 0)) {
        return 1;
    }

    /* check destination address */
    if (((addr_len == IEEE802154_SHORT_ADDRESS_LEN) &&
          (memcmp(&short_addr.u8, dst_addr, addr_len) == 0 ||
           memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0)) ||
        ((addr_len == IEEE802154_LONG_ADDRESS_LEN) &&
          (memcmp(ext_addr->uint8, dst_addr, addr_len) == 0))) {
        return 0;
    }

    return 1;
}
