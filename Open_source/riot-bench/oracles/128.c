size_t fmt_hex_bytes(uint8_t *out, const char *hex)
{
    size_t len = fmt_strlen(hex);

    if (len & 1) {
        return 0;
    }

    size_t final_len = len >> 1;

    if (out == NULL) {
        return final_len;
    }

    for (size_t i = 0, j = 0; j < final_len; i += 2, j++) {
        out[j] = fmt_hex_byte(hex + i);
    }

    return final_len;
}