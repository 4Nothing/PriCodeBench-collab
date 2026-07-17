uint16_t crc16_ccitt_fcs_finish(uint16_t crc, const unsigned char *buf, size_t len)
{
    return crc16_ccitt_kermit_update(crc, buf, len) ^ 0xFFFFU;
}

