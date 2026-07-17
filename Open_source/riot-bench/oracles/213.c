int isrpipe_read_all_timeout(isrpipe_t *isrpipe, uint8_t *buffer, size_t count, uint32_t timeout)
{
    uint8_t *pos = buffer;

    while (count) {
        int res = isrpipe_read_timeout(isrpipe, pos, count, timeout);
        if (res >= 0) {
            count -= res;
            pos += res;
        }
        else {
            return res;
        }
    }

    return pos - buffer;
}