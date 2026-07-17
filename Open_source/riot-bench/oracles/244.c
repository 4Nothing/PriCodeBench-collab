void reverse_buf(void *buf, size_t len)
{
    uint8_t *cur = buf;
    uint8_t *end = cur + len - 1;
    while (cur < end) {
        uint8_t tmp = *cur;
        *cur++ = *end;
        *end-- = tmp;
    }
}