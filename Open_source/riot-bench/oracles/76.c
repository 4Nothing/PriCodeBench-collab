uint16_t crc16_ccitt_fcs_start(const unsigned char *buf, size_t len)
{
    return crc16_ccitt_kermit_update(0xFFFF, buf, len);
}

