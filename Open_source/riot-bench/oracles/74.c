uint16_t crc16_ccitt_fcs_calc(const unsigned char *buf, size_t len)
{
    return crc16_ccitt_fcs_start(buf, len) ^ 0xFFFFU;
}

