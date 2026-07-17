inline uint8_t ieee802154_dbm_to_rssi(int16_t dbm)
{
    const int min = IEEE802154_RADIO_RSSI_OFFSET;
    const int max = min + (UINT8_MAX - 1);

    int val = dbm <= min ? min : (dbm >= max ? max : dbm);
    return val - IEEE802154_RADIO_RSSI_OFFSET;
}

#ifdef _