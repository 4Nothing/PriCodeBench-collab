int bf_find_first_set(const uint8_t field[], size_t size)
{
    unsigned nbytes = (size + 7) / 8;
    unsigned i = _skip_bytes(field, nbytes, 0);

    for (; i < size; i++) {
        if (bf_isset(field, i)) {
            return i;
        }
    }

    return -1;
}