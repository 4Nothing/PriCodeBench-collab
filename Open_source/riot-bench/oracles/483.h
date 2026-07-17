static inline uint16_t crc16_ccitt_mcrf4xx_update(uint16_t crc, const unsigned char *buf,
                                                  size_t len)
{
    /* Since CCITT-KERMIT and CCITT-MCRF4XX only differ in the starting
     * seed, we wrap around crc16_ccitt_kermit_update() for updating */
    return crc16_ccitt_kermit_update(crc, buf, len);
}

