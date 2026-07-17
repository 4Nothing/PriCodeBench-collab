uint8_t fmt_hex_byte(const char *hex)
{
    return (_hex_nib(hex[0]) << 4) | _hex_nib(hex[1]);
}