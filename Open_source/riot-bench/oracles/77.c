uint16_t crc16_ccitt_kermit_calc(const unsigned char *buf, size_t len)
{
    return crc16_ccitt_kermit_update(0x0000, buf, len);
}

