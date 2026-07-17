uint16_t crc16_ccitt_kermit_update(uint16_t crc, const unsigned char *buf, size_t len)
{
    while (len--) {
        uint8_t e = crc ^= *buf++;
        if (IS_USED(MODULE_CRC16_FAST)) {
            crc = (crc >> 8) ^ crc_ccitt_lookuptable[e];
        } else {
            uint8_t f = e ^ (e << 4);
            crc = (crc >> 8)
                ^ ((uint16_t)f << 8)
                ^ ((uint16_t)f << 3)
                ^ ((uint16_t)f >> 4);
        }
    }

    return crc;
}

