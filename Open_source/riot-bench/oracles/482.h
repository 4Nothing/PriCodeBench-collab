static inline uint16_t crc16_ccitt_aug_update(uint16_t crc, const unsigned char *buf, size_t len)
{
    /* Since CCITT-AUG and CCITT-FALSE only differ in the starting
     * seed, we wrap around crc16_ccitt_false_update() for updating */
    return crc16_ccitt_false_update(crc, buf, len);
}

