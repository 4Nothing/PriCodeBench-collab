inline int16_t ieee802154_rssi_to_dbm(uint8_t rssi)
{
    return rssi + IEEE802154_RADIO_RSSI_OFFSET;
}

/**
 * @