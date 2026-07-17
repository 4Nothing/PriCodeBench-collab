uint8_t crc8(const uint8_t *data, size_t len, uint8_t g_polynom, uint8_t crc)
{
    /* iterate over all bytes */
    for (size_t i=0; i < len; i++)
    {
        crc ^= data[i];

        for (int i = 0; i < 8; i++)
        {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}