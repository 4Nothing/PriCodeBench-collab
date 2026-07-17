uint16_t crc16_ccitt_aug_calc(const unsigned char *buf, size_t len)
{
    return crc16_ccitt_false_update(0x1D0F, buf, len);
}

