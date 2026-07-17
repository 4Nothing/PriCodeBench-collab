uint16_t crc16_ccitt_false_update(uint16_t crc, const unsigned char *buf, size_t len)
{
    while (len--) {
        crc = byteorder_swaps(crc) ^ *buf++;
        if (IS_USED(MODULE_CRC16_FAST)) {
            crc = (crc & 0xFF00) ^ _crc16_ccitt_false_lookuptable[crc & 0xFF];
        } else {
            crc ^= (uint8_t)(crc & 0xff) >> 4;
            crc ^= (crc << 8) << 4;
            crc ^= ((crc & 0xff) << 4) << 1;
        }
    }

    return crc;
}

