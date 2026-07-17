inline uint8_t ieee802154_get_seq(const uint8_t *mhr)
{
    return mhr[2];
}

/**
 * @